#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "test_support.h"
#include "glfw_input_fake.h"
#include "core/action_map.h"
#include "core/input_manager.h"
#include "core/resource_manager.h"
#include "app/game2D/cat.h"
#include "app/game2D/collision_system.h"
#include "app/game2D/game_data.h"
#include "app/game2D/player.h"
#include "app/game2D/projectile.h"
#include "app/game2D/projectile_manager.h"
#include "app/game2D/sprite.h"

#include <algorithm>
#include <array>
#include <initializer_list>
#include <memory>
#include <stdexcept>

namespace
{
using test_support::requireVec2;

/**
 * @brief Exercise real gameplay and resource lookup without loading GL functions.
 * @details Default textures have ID zero. Their names satisfy the actual Sprite
 * constructor, while animation and physics remain entirely CPU operations.
 * Local actors die before fixture cleanup; owned projectiles are destroyed before
 * clearing their texture references. No renderer, framebuffer, or shader is made.
 */
struct GameplayFixture
{
    glfw_input_fake::ScopedState fake;
    InputManager input;
    ActionMap actions{input};
    ProjectileManager projectiles;

    GameplayFixture()
    {
        ResourceManager::texturesMap.clear();
        for (const auto& name : {gameData::playerTexName, gameData::catTexName,
                                 gameData::arrowTexName, gameData::energyBallTexName})
            ResourceManager::texturesMap.try_emplace(name);

        for (int key : {GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_F})
            input.registerKey(key);
        actions.bind(Action::P1MoveLeft, GLFW_KEY_A);
        actions.bind(Action::P1MoveRight, GLFW_KEY_D);
        actions.bind(Action::P1Jump, GLFW_KEY_SPACE);
        actions.bind(Action::P1Attack, GLFW_KEY_F);
    }

    ~GameplayFixture()
    {
        projectiles.resetProjSys();
        ResourceManager::texturesMap.clear();
    }

    Player makePlayer(glm::vec2 position = gameData::playerPosition,
                      glm::vec2 size = gameData::playerSize,
                      glm::vec2 velocity = gameData::playerVelocity)
    {
        return Player(gameData::playerTexName, position, size, velocity);
    }

    Cat makeCat(glm::vec2 position = gameData::catPosition,
                glm::vec2 size = gameData::catSize,
                glm::vec2 velocity = gameData::catVelocity)
    {
        return Cat(gameData::catTexName, position, size, velocity);
    }

    /** @brief Drive the actual input transition code rather than assigning actor states. */
    void pollKeys(std::initializer_list<int> pressed)
    {
        for (int key : {GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_F})
            glfw_input_fake::setKey(
                key, std::find(pressed.begin(), pressed.end(), key) != pressed.end()
                         ? GLFW_PRESS : GLFW_RELEASE);
        input.update(nullptr);
    }

    void holdKeys(std::initializer_list<int> pressed)
    {
        pollKeys(pressed);
        input.update(nullptr);
    }

    void beginPlayerAttack(Player& player)
    {
        pollKeys({GLFW_KEY_F});
        player.inputHandler(actions, 0.0f, projectiles);
        REQUIRE(player.getState() == State::Attack);
        // Initialize the selected animation without skipping its first frame.
        player.update(0.0f, projectiles);
    }

    void advancePlayerToShootingFrame(Player& player)
    {
        const auto& animation = gameData::playerAnimations.at(State::Attack);
        for (int frame = animation.startFrame; frame < gameData::playerShootFrame; ++frame)
            player.update(1.0f / animation.fps, projectiles);
    }
};
} // namespace

TEST_CASE_METHOD(GameplayFixture, "Sprite resolves the actual texture map and advances animation on CPU",
                 "[gameplay][sprite][resources]")
{
    REQUIRE(glad_glGenTextures == nullptr);
    REQUIRE(glad_glCreateProgram == nullptr);
    REQUIRE(ResourceManager::shadersMap.empty());
    Sprite sprite(gameData::playerTexName, 6, 13);
    REQUIRE(std::addressof(sprite.getTex()) ==
            std::addressof(ResourceManager::getTexture(gameData::playerTexName)));
    REQUIRE(sprite.getTex().getID() == 0);
    sprite.setAnimation(2, 4, 3, 4.0f);
    sprite.updateTexture(0.0f, true);
    REQUIRE(sprite.getAnimator().getFrame() == 2);
    sprite.updateTexture(0.25f, false);
    REQUIRE(sprite.getAnimator().getFrame() == 3);
    REQUIRE_FALSE(sprite.getAnimator().isAnimationFinished());
    sprite.updateTexture(0.25f, true);
    REQUIRE(sprite.getAnimator().getFrame() == 4);
    REQUIRE(sprite.getAnimator().isAnimationFinished());
    REQUIRE_THROWS_AS(Sprite("missing_gameplay_texture"), std::runtime_error);
    REQUIRE(glad_glGenTextures == nullptr);
    REQUIRE(glad_glCreateProgram == nullptr);
}

TEST_CASE_METHOD(GameplayFixture, "Player constructor preserves its body and initializes the collision offset",
                 "[gameplay][player]")
{
    const auto player = makePlayer({-25.0f, 140.0f}, {80.0f, 120.0f}, {15.0f, -20.0f});
    requireVec2(player.getPosition(), {-25.0f, 140.0f});
    requireVec2(player.getSize(), {80.0f, 120.0f});
    requireVec2(player.getGameObj().getVelocity(), {15.0f, -20.0f});
    REQUIRE(player.getState() == State::Idle);
    REQUIRE(player.getTex().getID() == 0);
    requireVec2(player.getBB().m_min, player.getPosition() + gameData::bbOffsetPosition);
    requireVec2(player.getBB().m_size, gameData::bbSize);
}

/** @test Walking uses ongoing(), so a fresh press alone deliberately does not walk. */
TEST_CASE_METHOD(GameplayFixture, "Player legacy walking starts on held input in either direction",
                 "[gameplay][player][contract]")
{
    for (int key : {GLFW_KEY_D, GLFW_KEY_A})
    {
        CAPTURE(key);
        auto player = makePlayer({100.0f, gameData::groundLevel});
        pollKeys({key});
        player.inputHandler(actions, 0.0f, projectiles);
        REQUIRE(player.getState() == State::Idle);
        requireVec2(player.getGameObj().getVelocity(), glm::vec2(0.0f));
        input.update(nullptr);
        player.inputHandler(actions, 0.0f, projectiles);
        REQUIRE(player.getState() == State::Walk);
        const float expectedSpeed = key == GLFW_KEY_D ? 200.0f : -200.0f;
        REQUIRE(player.getGameObj().getHorizontalVelocity() == Catch::Approx(expectedSpeed));
        player.update(0.25f, projectiles);
        const float expectedX = key == GLFW_KEY_D ? 150.0f : 50.0f;
        requireVec2(player.getPosition(), {expectedX, gameData::groundLevel});
        requireVec2(player.getBB().m_min, player.getPosition() + gameData::bbOffsetPosition);
    }
}

TEST_CASE_METHOD(GameplayFixture, "Player legacy simultaneous walking inputs favor left and releasing stops movement",
                 "[gameplay][player][contract]")
{
    auto player = makePlayer({100.0f, gameData::groundLevel});
    holdKeys({GLFW_KEY_A, GLFW_KEY_D});
    player.inputHandler(actions, 0.0f, projectiles);
    REQUIRE(player.getState() == State::Walk);
    REQUIRE(player.getGameObj().getHorizontalVelocity() == Catch::Approx(-200.0f));
    player.update(0.25f, projectiles);
    requireVec2(player.getPosition(), {50.0f, gameData::groundLevel});
    pollKeys({});
    player.inputHandler(actions, 0.0f, projectiles);
    REQUIRE(player.getState() == State::Idle);
    requireVec2(player.getGameObj().getVelocity(), glm::vec2(0.0f));
    player.update(0.5f, projectiles);
    requireVec2(player.getPosition(), {50.0f, gameData::groundLevel});
}

/** @test A jump integrates the old velocity, then applies gravity, and cannot restart in air. */
TEST_CASE_METHOD(GameplayFixture, "Player jump accepts horizontal input rejects airborne attack and lands",
                 "[gameplay][player]")
{
    auto player = makePlayer();
    pollKeys({GLFW_KEY_SPACE});
    player.inputHandler(actions, 0.0f, projectiles);
    REQUIRE(player.getState() == State::Jump);
    REQUIRE(player.getGameObj().getVerticalVelocity() == Catch::Approx(600.0f));
    player.update(0.125f, projectiles);
    REQUIRE(player.getPosition().y == Catch::Approx(175.0f));
    REQUIRE(player.getGameObj().getVerticalVelocity() == Catch::Approx(450.0f));

    pollKeys({});
    pollKeys({GLFW_KEY_SPACE, GLFW_KEY_F});
    player.inputHandler(actions, 0.0f, projectiles);
    REQUIRE(player.getState() == State::Jump);
    REQUIRE(player.getGameObj().getVerticalVelocity() == Catch::Approx(450.0f));
    REQUIRE(projectiles.getProjectiles().empty());
    holdKeys({GLFW_KEY_D});
    player.inputHandler(actions, 0.0f, projectiles);
    REQUIRE(player.getState() == State::Jump);
    player.update(0.125f, projectiles);
    requireVec2(player.getPosition(), {25.0f, 231.25f});
    REQUIRE(player.getGameObj().getVerticalVelocity() == Catch::Approx(300.0f));

    for (int frame = 0; frame < 16 && player.getState() == State::Jump; ++frame)
        player.update(0.125f, projectiles);
    REQUIRE(player.getState() == State::Idle);
    REQUIRE(player.getPosition().y == Catch::Approx(gameData::groundLevel));
    REQUIRE(player.getGameObj().getVerticalVelocity() == Catch::Approx(0.0f));
    pollKeys({});
    player.inputHandler(actions, 0.0f, projectiles);
    requireVec2(player.getGameObj().getVelocity(), glm::vec2(0.0f));
}

TEST_CASE_METHOD(GameplayFixture, "Player attack emits once and rearms immediately",
                 "[gameplay][player][projectile][contract]")
{
    auto player = makePlayer();
    beginPlayerAttack(player);
    holdKeys({GLFW_KEY_D});
    player.inputHandler(actions, 0.0f, projectiles);
    REQUIRE(player.getState() == State::Attack);
    requireVec2(player.getGameObj().getVelocity(), glm::vec2(0.0f));

    const float frameTime = 1.0f / gameData::playerAnimations.at(State::Attack).fps;
    for (int frame = 2; frame < gameData::playerShootFrame; ++frame)
    {
        player.update(frameTime, projectiles);
        REQUIRE(projectiles.getProjectiles().empty());
    }
    player.update(frameTime, projectiles);
    REQUIRE(projectiles.getProjectiles().size() == 1);
    const auto& arrow = *projectiles.getProjectiles().front();
    REQUIRE(arrow.getOwner() == ProjectileOwner::Player);
    requireVec2(arrow.getPosition(), {55.0f, 128.0f});
    requireVec2(arrow.getSize(), gameData::arrowSize);
    requireVec2(arrow.getVelocity(), {600.0f, 0.0f});
    player.update(0.0f, projectiles);
    REQUIRE(projectiles.getProjectiles().size() == 1);

    for (int frame = gameData::playerShootFrame;
         frame < gameData::playerAnimations.at(State::Attack).endFrame; ++frame)
        player.update(frameTime, projectiles);
    REQUIRE(player.getState() == State::Idle);

    // The state transition selects Idle immediately, so an immediate new
    // attack starts from its first frame and emits a second arrow once.
    pollKeys({});beginPlayerAttack(player);REQUIRE(player.getState()==State::Attack);
    advancePlayerToShootingFrame(player);REQUIRE(projectiles.getProjectiles().size()==2);
}

TEST_CASE_METHOD(GameplayFixture, "Player facing left emits an arrow with negative speed from its left origin",
                 "[gameplay][player][projectile]")
{
    auto player = makePlayer({250.0f, gameData::groundLevel});
    holdKeys({GLFW_KEY_A});
    player.inputHandler(actions, 0.0f, projectiles);
    beginPlayerAttack(player);
    advancePlayerToShootingFrame(player);
    REQUIRE(projectiles.getProjectiles().size() == 1);
    const auto& arrow = *projectiles.getProjectiles().front();
    requireVec2(arrow.getPosition(), {250.0f, 128.0f});
    requireVec2(arrow.getVelocity(), {-600.0f, 0.0f});
    REQUIRE(arrow.getOwner() == ProjectileOwner::Player);
}

TEST_CASE_METHOD(GameplayFixture, "Player hit is deferred to update stops movement and finishes in Dead",
                 "[gameplay][player]")
{
    auto player = makePlayer({100.0f, 200.0f}, gameData::playerSize, {25.0f, 10.0f});
    player.hasBeenHit();
    REQUIRE(player.getState() == State::Idle);
    player.update(0.0f, projectiles);
    REQUIRE(player.getState() == State::Hit);
    REQUIRE(player.getPosition().y == Catch::Approx(gameData::groundLevel));
    requireVec2(player.getGameObj().getVelocity(), glm::vec2(0.0f));
    holdKeys({GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_F});
    player.inputHandler(actions, 0.0f, projectiles);
    REQUIRE(player.getState() == State::Hit);
    const auto& animation = gameData::playerAnimations.at(State::Hit);
    for (int frame = animation.startFrame; frame < animation.endFrame; ++frame)
        player.update(1.0f / animation.fps, projectiles);
    REQUIRE(player.getState() == State::Dead);
    player.update(0.0f, projectiles);
    REQUIRE(player.getState() == State::Dead);
    REQUIRE(projectiles.getProjectiles().empty());
}

/** @test Reset changes the body immediately; the existing BB is refreshed only by update(). */
TEST_CASE_METHOD(GameplayFixture, "Player reset restores controls and bounds immediately",
                 "[gameplay][player][contract]")
{
    auto player = makePlayer({300.0f, 200.0f});
    player.hasBeenHit();
    player.update(0.0f, projectiles);
    REQUIRE(player.getState() == State::Hit);
    const BoundingBox previousBox = player.getBB();
    player.resetPlayer();
    requireVec2(player.getPosition(), gameData::playerPosition);
    requireVec2(player.getGameObj().getVelocity(), glm::vec2(0.0f));
    REQUIRE(player.getState() == State::Idle);
    requireVec2(player.getBB().m_min,gameData::playerPosition+gameData::bbOffsetPosition);
    player.update(0.0f, projectiles);
    requireVec2(player.getBB().m_min, gameData::playerPosition + gameData::bbOffsetPosition);
    pollKeys({GLFW_KEY_SPACE});
    player.inputHandler(actions, 0.0f, projectiles);
    player.update(0.0f, projectiles);
    REQUIRE(player.getState() == State::Jump);
}

TEST_CASE_METHOD(GameplayFixture, "Cat starts with configured lives and hits saturate at zero",
                 "[gameplay][cat]")
{
    auto cat = makeCat({-50.0f, 140.0f}, {80.0f, 120.0f}, {20.0f, -10.0f});
    requireVec2(cat.getPosition(), {-50.0f, 140.0f});
    requireVec2(cat.getGameObj().getSize(), {80.0f, 120.0f});
    requireVec2(cat.getGameObj().getVelocity(), {20.0f, -10.0f});
    requireVec2(cat.getBB().m_min, cat.getPosition() + gameData::bbOffsetPosition);
    REQUIRE(cat.getState() == State::Idle);
    REQUIRE(cat.getLives() == gameData::catLives);
    for (int hit = 1; hit <= gameData::catLives + 2; ++hit)
    {
        CAPTURE(hit);
        cat.hasBeenHit();
        REQUIRE(cat.getLives() == std::max(0, gameData::catLives - hit));
    }
}

TEST_CASE_METHOD(GameplayFixture, "Cat attack timer requires strictly more than two seconds at long range",
                 "[gameplay][cat][boundary]")
{
    auto cat = makeCat();
    cat.autoInput(2.0f, 0.0f);
    REQUIRE(cat.getState() == State::Idle);
    cat.autoInput(1.0f / 1024.0f, 0.0f);
    REQUIRE(cat.getState() == State::Attack);
    REQUIRE(projectiles.getProjectiles().empty());
}

TEST_CASE_METHOD(GameplayFixture, "Cat jumps inside the proximity boundary or when a life charge is available",
                 "[gameplay][cat][boundary]")
{
    auto boundary = makeCat();
    boundary.autoInput(0.0f, 700.0f);
    REQUIRE(boundary.getState() == State::Idle);
    auto close = makeCat();
    close.autoInput(0.0f, 699.0f);
    REQUIRE(close.getState() == State::Jump);
    REQUIRE(close.getGameObj().getVerticalVelocity() == Catch::Approx(1000.0f));
    auto charged = makeCat();
    charged.hasBeenHit();
    charged.hasBeenHit();
    REQUIRE(charged.getLives() == 7);
    charged.autoInput(0.0f, 0.0f);
    REQUIRE(charged.getState() == State::Jump);
}

TEST_CASE_METHOD(GameplayFixture, "Cat jump chooses horizontal direction and applies gravity after integration",
                 "[gameplay][cat]")
{
    auto cat = makeCat();
    cat.autoInput(0.0f, 400.0f);
    REQUIRE(cat.getState() == State::Jump);
    cat.update(0.125f, projectiles, {400.0f, gameData::groundLevel}, 0.0f);
    requireVec2(cat.getPosition(), {500.0f, 225.0f});
    requireVec2(cat.getGameObj().getVelocity(), {-300.0f, 850.0f});
    cat.autoInput(3.0f, 1000.0f);
    REQUIRE(cat.getState() == State::Jump);
    cat.update(0.125f, projectiles, {400.0f, gameData::groundLevel}, 0.0f);
    requireVec2(cat.getPosition(), {462.5f, 331.25f});
    requireVec2(cat.getGameObj().getVelocity(), {-300.0f, 700.0f});
}

/** @test Drive the public jump handler at the apex, without replaying its trajectory algorithm. */
TEST_CASE_METHOD(GameplayFixture, "Cat apex blast waits past each half second emits four volleys then lands",
                 "[gameplay][cat][projectile][boundary]")
{
    auto cat = makeCat({300.0f, gameData::groundLevel});
    cat.autoInput(0.0f, 400.0f);
    cat.getGameObj().setPosition({300.0f, 400.0f});
    cat.getGameObj().setVelocity({0.0f, -1.0f});
    std::size_t total = 0;
    for (int volleySize : {3, 4, 5, 6})
    {
        CAPTURE(volleySize);
        cat.jumpMoveHandler(0.5f, projectiles, 400.0f, 0.0f);
        REQUIRE(projectiles.getProjectiles().size() == total);
        requireVec2(cat.getGameObj().getVelocity(), glm::vec2(0.0f));
        cat.jumpMoveHandler(1.0f / 1024.0f, projectiles, 400.0f, 0.0f);
        REQUIRE(projectiles.getProjectiles().size() == total + volleySize);
        const float expectedSpeed = gameData::energyBallSpeed -
                                    gameData::blastSpeedAttenuation * volleySize;
        for (std::size_t index = total; index < total + volleySize; ++index)
        {
            const auto& ball = *projectiles.getProjectiles()[index];
            REQUIRE(ball.getOwner() == ProjectileOwner::Cat);
            requireVec2(ball.getPosition(), {350.0f, 400.0f});
            requireVec2(ball.getSize(), gameData::energyBallSize);
            REQUIRE(glm::length(ball.getVelocity()) ==
                    Catch::Approx(expectedSpeed).epsilon(1e-5f));
            REQUIRE(ball.getVelocity().y < 0.0f);
        }
        total += volleySize;
    }
    REQUIRE(total == 18);
    cat.jumpMoveHandler(0.01f, projectiles, 400.0f, 0.0f);
    REQUIRE(cat.getGameObj().getHorizontalVelocity() == Catch::Approx(-300.0f));
    cat.getGameObj().setVerticalPosition(gameData::groundLevel - 0.25f);
    cat.jumpMoveHandler(0.0f, projectiles, 400.0f, 0.0f);
    REQUIRE(cat.getState() == State::Idle);
    REQUIRE(cat.getPosition().y == Catch::Approx(gameData::groundLevel));
    requireVec2(cat.getGameObj().getVelocity(), glm::vec2(0.0f));
    cat.autoInput(2.0f, 1000.0f);
    REQUIRE(cat.getState() == State::Idle);
}

/** @test Facing is observable through spawn position; aim uses the body origin, not the spawn offset. */
TEST_CASE_METHOD(GameplayFixture, "Cat attack emits once from the facing side and aims from its body origin",
                 "[gameplay][cat][projectile]")
{
    for (float targetX : {800.0f, 0.0f})
    {
        CAPTURE(targetX);
        projectiles.resetProjSys();
        auto cat = makeCat();
        const glm::vec2 target(targetX, gameData::groundLevel);
        cat.autoInput(2.01f, targetX);
        REQUIRE(cat.getState() == State::Attack);
        cat.update(0.0f, projectiles, target, 0.0f);
        const float frameTime = 1.0f / gameData::catAnimations.at(State::Attack).fps;
        for (int frame = 2; frame < gameData::catShootFrame; ++frame)
        {
            cat.update(frameTime, projectiles, target, 0.0f);
            REQUIRE(projectiles.getProjectiles().empty());
        }
        cat.update(frameTime, projectiles, target, 0.0f);
        REQUIRE(projectiles.getProjectiles().size() == 1);
        const auto& ball = *projectiles.getProjectiles().front();
        REQUIRE(ball.getOwner() == ProjectileOwner::Cat);
        requireVec2(ball.getPosition(), {targetX > 500.0f ? 600.0f : 480.0f, 112.0f});
        requireVec2(ball.getVelocity(), {targetX > 500.0f ? 600.0f : -600.0f, 0.0f});
        cat.update(0.0f, projectiles, target, 0.0f);
        REQUIRE(projectiles.getProjectiles().size() == 1);
        for (int frame = gameData::catShootFrame;
             frame < gameData::catAnimations.at(State::Attack).endFrame; ++frame)
            cat.update(frameTime, projectiles, target, 0.0f);
        REQUIRE(cat.getState() == State::Idle);
    }
}

TEST_CASE_METHOD(GameplayFixture, "Cat reset restores lives timers flags and bounds immediately",
                 "[gameplay][cat][contract]")
{
    auto cat = makeCat({600.0f, 200.0f});
    cat.autoInput(2.01f, 0.0f);
    for (int hit = 0; hit < gameData::catLives; ++hit)
        cat.hasBeenHit();
    cat.update(0.0f, projectiles, {0.0f, gameData::groundLevel}, 0.0f);
    REQUIRE(cat.getState() == State::Hit);
    const BoundingBox previousBox = cat.getBB();
    cat.resetCat();
    requireVec2(cat.getPosition(), gameData::catPosition);
    requireVec2(cat.getGameObj().getVelocity(), glm::vec2(0.0f));
    REQUIRE(cat.getLives() == gameData::catLives);
    REQUIRE(cat.getState() == State::Idle);
    requireVec2(cat.getBB().m_min,gameData::catPosition+gameData::bbOffsetPosition);
    cat.update(0.0f, projectiles, {0.0f, gameData::groundLevel}, 0.0f);
    requireVec2(cat.getBB().m_min, gameData::catPosition + gameData::bbOffsetPosition);
    cat.autoInput(2.0f, 0.0f);
    REQUIRE(cat.getState() == State::Idle);
    cat.autoInput(0.01f, 0.0f);
    REQUIRE(cat.getState() == State::Attack);
}

/** @test Death remains terminal until an explicit reset. */
TEST_CASE_METHOD(GameplayFixture, "Cat death is terminal through subsequent input and updates",
                 "[gameplay][cat][contract]")
{
    auto cat = makeCat();
    for (int hit = 0; hit < gameData::catLives; ++hit)
        cat.hasBeenHit();
    const glm::vec2 target(0.0f, gameData::groundLevel);
    cat.update(0.0f, projectiles, target, 0.0f);
    REQUIRE(cat.getState() == State::Hit);
    const auto& animation = gameData::catAnimations.at(State::Hit);
    for (int frame = animation.startFrame; frame < animation.endFrame; ++frame)
        cat.update(1.0f / animation.fps, projectiles, target, 0.0f);
    REQUIRE(cat.getState() == State::Dead);
    cat.autoInput(3.0f, target.x);
    REQUIRE(cat.getState() == State::Dead);
    cat.update(0.0f, projectiles, target, 0.0f);
    REQUIRE(cat.getState() == State::Dead);
    REQUIRE(cat.getLives() == 0);
}

TEST_CASE_METHOD(GameplayFixture, "Projectile stores owner and supplied signed velocity independently of facing",
                 "[gameplay][projectile]")
{
    for (ProjectileOwner owner : {ProjectileOwner::Player, ProjectileOwner::Cat})
    {
        CAPTURE(static_cast<int>(owner));
        Projectile projectile(gameData::arrowTexName, {-3.0f, 4.0f}, {4.0f, 2.0f},
                              {8.0f, -12.0f}, false, owner);
        REQUIRE(projectile.getOwner() == owner);
        REQUIRE(projectile.getStatus());
        requireVec2(projectile.getSize(), {4.0f, 2.0f});
        requireVec2(projectile.getGameObj().getVelocity(), {8.0f, -12.0f});
        projectile.update(0.25f);
        requireVec2(projectile.getPosition(), {-1.0f, 1.0f});
        projectile.update(-0.25f);
        requireVec2(projectile.getPosition(), {-3.0f, 4.0f});
    }
}

TEST_CASE_METHOD(GameplayFixture, "Inactive projectiles stay still until cleanup",
                 "[gameplay][projectile][contract]")
{
    Projectile projectile(gameData::energyBallTexName, {10.0f, 20.0f}, {2.0f, 2.0f},
                          {-4.0f, 8.0f}, true, ProjectileOwner::Cat);
    projectile.setStatus(false);
    REQUIRE_FALSE(projectile.getStatus());
    projectile.update(0.5f);
    requireVec2(projectile.getPosition(),{10.0f,20.0f});
    projectile.setStatus(true);
    REQUIRE(projectile.getStatus());
}

/** @test Cancellation is based on origin coordinates, with inclusive camera limits. */
TEST_CASE_METHOD(GameplayFixture, "Projectile manager retains exact bounds and removes adjacent inactive and outside projectiles",
                 "[gameplay][projectile-manager][boundary]")
{
    const std::array<glm::vec2, 8> positions{{
        {-10.0f, 10.0f}, {10.0f, 10.0f}, {0.0f, 0.0f}, {0.0f, 20.0f},
        {-11.0f, 10.0f}, {11.0f, 10.0f}, {0.0f, -1.0f}, {0.0f, 21.0f}}};
    std::array<Projectile*, 4> survivors{};
    for (std::size_t index = 0; index < positions.size(); ++index)
    {
        projectiles.emit(gameData::arrowTexName, positions[index], {100.0f, 100.0f},
                         glm::vec2(0.0f), true, ProjectileOwner::Player);
        if (index < survivors.size())
            survivors[index] = projectiles.getProjectiles().back().get();
    }
    for (int index = 0; index < 2; ++index)
    {
        projectiles.emit(gameData::energyBallTexName, {0.0f, 10.0f}, {2.0f, 2.0f},
                         glm::vec2(0.0f), false, ProjectileOwner::Cat);
        projectiles.getProjectiles().back()->setStatus(false);
    }
    projectiles.update(0.0f, 10.0f, -10.0f, 20.0f, 0.0f);
    REQUIRE(projectiles.getProjectiles().size() == 4);
    for (std::size_t index = 0; index < survivors.size(); ++index)
    {
        CAPTURE(index);
        REQUIRE(projectiles.getProjectiles()[index].get() == survivors[index]);
        requireVec2(projectiles.getProjectiles()[index]->getPosition(), positions[index]);
        REQUIRE(projectiles.getProjectiles()[index]->getStatus());
    }
}

TEST_CASE_METHOD(GameplayFixture, "Projectile manager integrates before cancellation and can be reset and reused",
                 "[gameplay][projectile-manager]")
{
    projectiles.emit(gameData::arrowTexName, {0.0f, 5.0f}, {2.0f, 2.0f},
                     {20.0f, 0.0f}, true, ProjectileOwner::Player);
    projectiles.emit(gameData::energyBallTexName, {0.0f, 5.0f}, {2.0f, 2.0f},
                     {22.0f, 0.0f}, true, ProjectileOwner::Cat);
    projectiles.update(0.5f, 10.0f, -10.0f, 20.0f, 0.0f);
    REQUIRE(projectiles.getProjectiles().size() == 1);
    REQUIRE(projectiles.getProjectiles().front()->getOwner() == ProjectileOwner::Player);
    requireVec2(projectiles.getProjectiles().front()->getPosition(), {10.0f, 5.0f});
    projectiles.resetProjSys();
    projectiles.resetProjSys();
    projectiles.update(0.5f, 10.0f, -10.0f, 20.0f, 0.0f);
    REQUIRE(projectiles.getProjectiles().empty());
    projectiles.emit(gameData::energyBallTexName, {-2.0f, 5.0f}, {2.0f, 2.0f},
                     {-4.0f, 0.0f}, false, ProjectileOwner::Cat);
    projectiles.update(0.5f, 10.0f, -10.0f, 20.0f, 0.0f);
    REQUIRE(projectiles.getProjectiles().size() == 1);
    requireVec2(projectiles.getProjectiles().front()->getPosition(), {-4.0f, 5.0f});
}

TEST_CASE_METHOD(GameplayFixture, "Collision ownership selects the opposing actor and defers player Hit until update",
                 "[gameplay][collision]")
{
    auto player = makePlayer();
    auto cat = makeCat();
    const glm::vec2 playerPoint = player.getBB().m_min + glm::vec2(1.0f);
    const glm::vec2 catPoint = cat.getBB().m_min + glm::vec2(1.0f);
    projectiles.emit(gameData::arrowTexName, playerPoint, {2.0f, 2.0f},
                     glm::vec2(0.0f), true, ProjectileOwner::Player);
    projectiles.emit(gameData::energyBallTexName, catPoint, {2.0f, 2.0f},
                     glm::vec2(0.0f), false, ProjectileOwner::Cat);
    CollisionSystem collisions(player, cat, projectiles);
    collisions.checkCollision();
    REQUIRE(cat.getLives() == gameData::catLives);
    player.update(0.0f, projectiles);
    REQUIRE(player.getState() == State::Idle);
    REQUIRE(projectiles.getProjectiles()[0]->getStatus());
    REQUIRE(projectiles.getProjectiles()[1]->getStatus());

    projectiles.emit(gameData::arrowTexName, catPoint, {2.0f, 2.0f},
                     glm::vec2(0.0f), true, ProjectileOwner::Player);
    projectiles.emit(gameData::energyBallTexName, playerPoint, {2.0f, 2.0f},
                     glm::vec2(0.0f), false, ProjectileOwner::Cat);
    collisions.checkCollision();
    REQUIRE(cat.getLives() == gameData::catLives - 1);
    REQUIRE_FALSE(projectiles.getProjectiles()[2]->getStatus());
    REQUIRE_FALSE(projectiles.getProjectiles()[3]->getStatus());
    REQUIRE(player.getState() == State::Idle);
    player.update(0.0f, projectiles);
    REQUIRE(player.getState() == State::Hit);
    REQUIRE(projectiles.getProjectiles().size() == 4);
    projectiles.update(0.0f, 2000.0f, -2000.0f, 2000.0f, -2000.0f);
    REQUIRE(projectiles.getProjectiles().size() == 2);
}

TEST_CASE_METHOD(GameplayFixture, "Collision ignores projectile edge contact and spatial misses",
                 "[gameplay][collision][boundary]")
{
    auto player = makePlayer();
    auto cat = makeCat();
    const auto catBox = cat.getBB();
    const auto playerBox = player.getBB();
    projectiles.emit(gameData::arrowTexName, {catBox.m_max.x, catBox.m_min.y}, {2.0f, 2.0f},
                     glm::vec2(0.0f), true, ProjectileOwner::Player);
    projectiles.emit(gameData::energyBallTexName, {playerBox.m_min.x, playerBox.m_max.y},
                     {2.0f, 2.0f}, glm::vec2(0.0f), false, ProjectileOwner::Cat);
    projectiles.emit(gameData::arrowTexName, {-500.0f, -500.0f}, {2.0f, 2.0f},
                     glm::vec2(0.0f), true, ProjectileOwner::Player);
    CollisionSystem collisions(player, cat, projectiles);
    collisions.checkCollision();
    REQUIRE(cat.getLives() == gameData::catLives);
    player.update(0.0f, projectiles);
    REQUIRE(player.getState() == State::Idle);
    for (const auto& projectile : projectiles.getProjectiles())
        REQUIRE(projectile->getStatus());
}

/** @test CollisionSystem does not filter inactive projectiles; manager cleanup ends repeat damage. */
TEST_CASE_METHOD(GameplayFixture, "Collision ignores inactive projectiles before manager cleanup",
                 "[gameplay][collision][contract]")
{
    auto player = makePlayer();
    auto cat = makeCat();
    projectiles.emit(gameData::arrowTexName, cat.getBB().m_min + glm::vec2(1.0f),
                     {2.0f, 2.0f}, glm::vec2(0.0f), true, ProjectileOwner::Player);
    projectiles.getProjectiles().front()->setStatus(false);
    CollisionSystem collisions(player, cat, projectiles);
    collisions.checkCollision();
    REQUIRE(cat.getLives() == gameData::catLives);
    collisions.checkCollision();
    REQUIRE(cat.getLives() == gameData::catLives);
    REQUIRE_FALSE(projectiles.getProjectiles().front()->getStatus());
    projectiles.update(0.0f, 2000.0f, -2000.0f, 2000.0f, -2000.0f);
    REQUIRE(projectiles.getProjectiles().empty());
    collisions.checkCollision();
    REQUIRE(cat.getLives() == gameData::catLives);
}

TEST_CASE_METHOD(GameplayFixture, "Multiple player projectile collisions saturate cat lives and start its Hit state",
                 "[gameplay][collision][cat]")
{
    auto player = makePlayer();
    auto cat = makeCat();
    for (int index = 0; index < gameData::catLives + 2; ++index)
        projectiles.emit(gameData::arrowTexName, cat.getBB().m_min + glm::vec2(1.0f),
                         {2.0f, 2.0f}, glm::vec2(0.0f), true, ProjectileOwner::Player);
    CollisionSystem collisions(player, cat, projectiles);
    collisions.checkCollision();
    REQUIRE(cat.getLives() == 0);
    REQUIRE(cat.getState() == State::Idle);
    for (const auto& projectile : projectiles.getProjectiles())
        REQUIRE_FALSE(projectile->getStatus());
    cat.update(0.0f, projectiles, player.getPosition(), 0.0f);
    REQUIRE(cat.getState() == State::Hit);
    REQUIRE(player.getState() == State::Idle);
    projectiles.update(0.0f, 2000.0f, -2000.0f, 2000.0f, -2000.0f);
    REQUIRE(projectiles.getProjectiles().empty());
}
