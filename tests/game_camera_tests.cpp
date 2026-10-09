#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../src/graphics/camera.h"
#include "../src/app/scene3D/scene_data.h"
#include "../src/app/game2D/animator.h"
#include "../src/app/game2D/game_obj.h"
#include "../src/app/game2D/state.h"

#include <array>
#include <cmath>
#include <string_view>

namespace
{
constexpr float pi = 3.14159265358979323846f;

Catch::Approx nearFloat(float value)
{
    // Absolute tolerance covers zero entries; relative tolerance covers nonzero floats.
    return Catch::Approx(value).margin(1.0e-5f).epsilon(1.0e-5f);
}

template <glm::length_t N>
void requireVector(const glm::vec<N, float>& actual, const glm::vec<N, float>& expected)
{
    for (glm::length_t axis = 0; axis < N; ++axis)
    {
        INFO("vector axis " << axis);
        REQUIRE(actual[axis] == nearFloat(expected[axis]));
    }
}

void requireMatrix(const glm::mat4& actual, const glm::mat4& expected)
{
    for (int column = 0; column < 4; ++column)
    {
        for (int row = 0; row < 4; ++row)
        {
            INFO("matrix column " << column << ", row " << row);
            REQUIRE(actual[column][row] == nearFloat(expected[column][row]));
        }
    }
}

Camera tiltedCamera()
{
    return Camera(glm::vec3(3.0f, 4.0f, 0.0f), 60.0f, 2.0f, 0.5f, 20.0f);
}

glm::mat4 tiltedView()
{
    // Analytic basis: right=(0,0,-1), up=(-4/5,3/5,0), back=(3/5,4/5,0).
    glm::mat4 view(0.0f);
    view[0] = glm::vec4(0.0f, -0.8f, 0.6f, 0.0f);
    view[1] = glm::vec4(0.0f, 0.6f, 0.8f, 0.0f);
    view[2] = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    view[3] = glm::vec4(0.0f, 0.0f, -5.0f, 1.0f);
    return view;
}
}

TEST_CASE("Default camera exposes scene configuration and a horizontal initial basis", "[camera]")
{
    // WHY: The default front is horizontal along -X rather than aimed down at the origin.
    const Camera camera;
    requireVector(camera.getPosition(), sceneData::cameraStartingPosition);
    requireVector(camera.getFront(), glm::vec3(-1.0f, 0.0f, 0.0f));
    requireVector(camera.getTarget(), sceneData::cameraStartingPosition + glm::vec3(-1.0f, 0.0f, 0.0f));
    REQUIRE(camera.getYaw() == nearFloat(-pi / 2.0f));
    REQUIRE(camera.getPitch() == nearFloat(0.0f));
    REQUIRE(camera.getMouseSpeed() == nearFloat(sceneData::cameraInitMouseSpeed));
    REQUIRE(camera.getKeyboardSpeed() == nearFloat(sceneData::cameraInitKeyboardSpeed));
    REQUIRE(camera.getGrounded() == sceneData::cameraInitialGrounded);

    glm::mat4 view(0.0f);
    view[0] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    view[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    view[2] = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    view[3] = glm::vec4(sceneData::cameraStartingPosition.z,
                        -sceneData::cameraStartingPosition.y,
                        -sceneData::cameraStartingPosition.x, 1.0f);
    requireMatrix(camera.getViewMatrix(), view);

    const float focal = 1.0f / std::tan(sceneData::cameraFovy * pi / 360.0f);
    const float nearPlane = sceneData::cameraNearPlane;
    const float farPlane = sceneData::cameraFarPlane;
    glm::mat4 projection(0.0f);
    projection[0][0] = focal / sceneData::cameraAspect;
    projection[1][1] = focal;
    projection[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    projection[2][3] = -1.0f;
    projection[3][2] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
    requireMatrix(camera.getPerspectiveProjMatrix(), projection);
}

TEST_CASE("Parametric camera aims at the origin with angles in radians", "[camera]")
{
    // WHY: A 3-4-5 triangle supplies independent expected direction and orientation.
    const Camera camera = tiltedCamera();
    requireVector(camera.getPosition(), glm::vec3(3.0f, 4.0f, 0.0f));
    requireVector(camera.getFront(), glm::vec3(-0.6f, -0.8f, 0.0f));
    requireVector(camera.getTarget(), glm::vec3(2.4f, 3.2f, 0.0f));
    REQUIRE(camera.getYaw() == nearFloat(-pi / 2.0f));
    REQUIRE(camera.getPitch() == nearFloat(std::asin(-0.8f)));
    REQUIRE(camera.getMouseSpeed() == nearFloat(sceneData::cameraInitMouseSpeed));
    REQUIRE(camera.getKeyboardSpeed() == nearFloat(sceneData::cameraInitKeyboardSpeed));
    REQUIRE(camera.getGrounded() == sceneData::cameraInitialGrounded);
}

TEST_CASE("Perspective and aspect setter preserve analytic clip coordinates", "[camera]")
{
    // WHY: Exact 90-degree frustum coefficients reveal degree, aspect, transpose, and depth errors.
    Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), 90.0f, 2.0f, 2.0f, 10.0f);
    glm::mat4 expected(0.0f);
    expected[0][0] = 0.5f;
    expected[1][1] = 1.0f;
    expected[2][2] = -1.5f;
    expected[2][3] = -1.0f;
    expected[3][2] = -5.0f;
    requireMatrix(camera.getPerspectiveProjMatrix(), expected);
    const glm::vec4 nearClip = camera.getPerspectiveProjMatrix() * glm::vec4(0.0f, 0.0f, -2.0f, 1.0f);
    const glm::vec4 farClip = camera.getPerspectiveProjMatrix() * glm::vec4(0.0f, 0.0f, -10.0f, 1.0f);
    REQUIRE(nearClip.z / nearClip.w == nearFloat(-1.0f));
    REQUIRE(farClip.z / farClip.w == nearFloat(1.0f));
    camera.setAspect(0.5f);
    expected[0][0] = 2.0f;
    requireMatrix(camera.getPerspectiveProjMatrix(), expected);
    requireVector(camera.getPosition(), glm::vec3(0.0f, 0.0f, 5.0f));
    requireVector(camera.getFront(), glm::vec3(0.0f, 0.0f, -1.0f));
}

TEST_CASE("Orthographic projection maps viewport corners and fixed depth", "[camera]")
{
    // WHY: Non-square dimensions catch swapped axes and incorrect pixel-to-clip translation.
    const Camera camera = tiltedCamera();
    glm::mat4 expected(0.0f);
    expected[0][0] = 0.0025f;
    expected[1][1] = 1.0f / 300.0f;
    expected[2][2] = -1.0f;
    expected[3] = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);
    const glm::mat4 projection = camera.getOrthoProj(800, 600);
    requireMatrix(projection, expected);
    requireVector(projection * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f), glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f));
    requireVector(projection * glm::vec4(800.0f, 600.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, -1.0f, 1.0f));
    requireVector(projection * glm::vec4(400.0f, 300.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

TEST_CASE("Tilted view and position setter preserve orientation", "[camera]")
{
    // WHY: A tilted analytic view catches basis errors; translation must not retarget the eye.
    Camera camera = tiltedCamera();
    glm::mat4 expected = tiltedView();
    requireMatrix(camera.getViewMatrix(), expected);
    requireVector(camera.getViewMatrix() * glm::vec4(3.0f, 4.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    requireVector(camera.getViewMatrix() * glm::vec4(2.4f, 3.2f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
    camera.setPosition(glm::vec3(8.0f, -2.0f, 5.0f));
    requireVector(camera.getPosition(), glm::vec3(8.0f, -2.0f, 5.0f));
    requireVector(camera.getFront(), glm::vec3(-0.6f, -0.8f, 0.0f));
    requireVector(camera.getTarget(), glm::vec3(7.4f, -2.8f, 5.0f));
    REQUIRE(camera.getYaw() == nearFloat(-pi / 2.0f));
    REQUIRE(camera.getPitch() == nearFloat(std::asin(-0.8f)));
    expected[3] = glm::vec4(5.0f, 7.6f, -3.2f, 1.0f);
    requireMatrix(camera.getViewMatrix(), expected);
}

TEST_CASE("Front setter updates angles view and movement basis", "[camera]")
{
    // WHY: A valid new unit front must affect rendering and strafing immediately.
    Camera camera = tiltedCamera();
    camera.setGrounded(0.0f);
    camera.setFront(glm::vec3(0.6f, 0.8f, 0.0f));
    requireVector(camera.getFront(), glm::vec3(0.6f, 0.8f, 0.0f));
    requireVector(camera.getTarget(), glm::vec3(3.6f, 4.8f, 0.0f));
    REQUIRE(camera.getYaw() == nearFloat(pi / 2.0f));
    REQUIRE(camera.getPitch() == nearFloat(std::asin(0.8f)));
    glm::mat4 expected(0.0f);
    expected[0] = glm::vec4(0.0f, -0.8f, -0.6f, 0.0f);
    expected[1] = glm::vec4(0.0f, 0.6f, -0.8f, 0.0f);
    expected[2] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    expected[3] = glm::vec4(0.0f, 0.0f, 5.0f, 1.0f);
    requireMatrix(camera.getViewMatrix(), expected);
    const float distance = 2.0f * camera.getKeyboardSpeed();
    SECTION("Forward")
    {
        camera.moveForward(2.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f + 0.6f * distance, 4.0f + 0.8f * distance, 0.0f));
    }
    SECTION("Right")
    {
        camera.moveRight(2.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f, 4.0f, distance));
    }
    SECTION("Up")
    {
        camera.moveUp(2.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f - 0.8f * distance, 4.0f + 0.6f * distance, 0.0f));
    }
}

TEST_CASE("All camera translations use signed local-basis distances", "[camera]")
{
    // WHY: A tilted front separates local up from world Y and exposes every movement sign.
    struct Movement { const char* name; void (Camera::*apply)(float); glm::vec3 direction; };
    const std::array<Movement, 6> movements{{
        {"forward", &Camera::moveForward, glm::vec3(-0.6f, -0.8f, 0.0f)},
        {"backward", &Camera::moveBackward, glm::vec3(0.6f, 0.8f, 0.0f)},
        {"right", &Camera::moveRight, glm::vec3(0.0f, 0.0f, -1.0f)},
        {"left", &Camera::moveLeft, glm::vec3(0.0f, 0.0f, 1.0f)},
        {"up", &Camera::moveUp, glm::vec3(-0.8f, 0.6f, 0.0f)},
        {"down", &Camera::moveDown, glm::vec3(0.8f, -0.6f, 0.0f)}
    }};
    for (const Movement& movement : movements)
    {
        for (float input : {0.0f, 2.0f, -2.0f})
        {
            INFO(movement.name << ", input " << input);
            Camera camera = tiltedCamera();
            camera.setGrounded(0.0f);
            (camera.*movement.apply)(input);
            const glm::vec3 expected = glm::vec3(3.0f, 4.0f, 0.0f)
                                    + movement.direction * (camera.getKeyboardSpeed() * input);
            requireVector(camera.getPosition(), expected);
            requireVector(camera.getTarget(), expected + glm::vec3(-0.6f, -0.8f, 0.0f));
            requireVector(camera.getFront(), glm::vec3(-0.6f, -0.8f, 0.0f));
        }
    }
}

TEST_CASE("Grounding uses a saved altitude and only constrains forward and backward", "[camera]")
{
    // WHY: Setter order and local vertical movement expose the legacy saved-height caveat.
    Camera camera = tiltedCamera();
    camera.setGrounded(2.5f);
    REQUIRE(camera.getGrounded());
    camera.setGrounded(-0.25f);
    REQUIRE(camera.getGrounded());
    camera.setGrounded(0.0f);
    REQUIRE_FALSE(camera.getGrounded());
    requireVector(camera.getPosition(), glm::vec3(3.0f, 4.0f, 0.0f));
    camera.setGrounded(1.0f);
    SECTION("Constructor altitude applies even to zero-distance forward")
    {
        camera.moveForward(0.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f, sceneData::cameraAltitude, 0.0f));
    }
    SECTION("Constructor altitude applies even to zero-distance backward")
    {
        camera.moveBackward(0.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f, sceneData::cameraAltitude, 0.0f));
    }
    SECTION("Front updates capture height while position setters do not")
    {
        camera.setPosition(glm::vec3(3.0f, 8.0f, 0.0f));
        camera.setFront(glm::vec3(-0.6f, -0.8f, 0.0f));
        camera.setPosition(glm::vec3(3.0f, 12.0f, 0.0f));
        camera.moveForward(2.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f - 1.2f * camera.getKeyboardSpeed(), 8.0f, 0.0f));
    }
    SECTION("Zero mouse offsets capture a new altitude")
    {
        camera.setPosition(glm::vec3(3.0f, 8.0f, 0.0f));
        camera.processMouseInputs(0.0f, 0.0f);
        camera.setPosition(glm::vec3(3.0f, 12.0f, 0.0f));
        camera.moveBackward(2.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f + 1.2f * camera.getKeyboardSpeed(), 8.0f, 0.0f));
    }
    SECTION("Up and down change height until forward or backward restores it")
    {
        camera.setFront(glm::vec3(-0.6f, -0.8f, 0.0f));
        const float distance = 2.0f * camera.getKeyboardSpeed();
        camera.moveUp(2.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f - 0.8f * distance, 4.0f + 0.6f * distance, 0.0f));
        camera.moveForward(0.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f - 0.8f * distance, 4.0f, 0.0f));
        camera.moveDown(2.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f, 4.0f - 0.6f * distance, 0.0f));
        camera.moveBackward(0.0f);
        requireVector(camera.getPosition(), glm::vec3(3.0f, 4.0f, 0.0f));
    }
}

TEST_CASE("Standalone rotations defer vectors and pitch clamping until mouse processing", "[camera]")
{
    // WHY: Direct angle setters leave view/movement stale and do not clamp the pitch.
    Camera camera = tiltedCamera();
    camera.setGrounded(0.0f);
    camera.rotateOnXAxis(125.0f);
    camera.rotateOnYAxis(250.0f);
    const float yaw = -pi / 2.0f - 125.0f * camera.getMouseSpeed();
    const float pitch = std::asin(-0.8f) + 250.0f * camera.getMouseSpeed();
    REQUIRE(camera.getYaw() == nearFloat(yaw));
    REQUIRE(camera.getPitch() == nearFloat(pitch));
    requireVector(camera.getFront(), glm::vec3(-0.6f, -0.8f, 0.0f));
    requireMatrix(camera.getViewMatrix(), tiltedView());
    camera.moveForward(2.0f);
    requireVector(camera.getPosition(), glm::vec3(3.0f - 1.2f * camera.getKeyboardSpeed(),
                                                4.0f - 1.6f * camera.getKeyboardSpeed(), 0.0f));
    camera.processMouseInputs(0.0f, 0.0f);
    const glm::vec3 front(std::cos(pitch) * std::sin(yaw),
                           std::sin(pitch), std::cos(pitch) * std::cos(yaw));
    requireVector(camera.getFront(), front);
    camera.rotateOnYAxis(4000.0f);
    REQUIRE(camera.getPitch() == nearFloat(pitch + 4000.0f * camera.getMouseSpeed()));
    requireVector(camera.getFront(), front);
    camera.processMouseInputs(0.0f, 0.0f);
    REQUIRE(camera.getPitch() == nearFloat(89.0f * pi / 180.0f));
}

TEST_CASE("Mouse input rebuilds an analytic elevated view and movement basis", "[camera]")
{
    // WHY: Known zero yaw and 30-degree elevation verify sensitivity and all refreshed basis axes.
    Camera camera(glm::vec3(5.0f, 0.0f, 0.0f), 90.0f, 2.0f, 2.0f, 10.0f);
    camera.setGrounded(0.0f);
    camera.processMouseInputs(-pi / (2.0f * camera.getMouseSpeed()), pi / (6.0f * camera.getMouseSpeed()));
    constexpr float cosine30 = 0.8660254038f;
    REQUIRE(camera.getYaw() == nearFloat(0.0f));
    REQUIRE(camera.getPitch() == nearFloat(pi / 6.0f));
    requireVector(camera.getFront(), glm::vec3(0.0f, 0.5f, cosine30));
    requireVector(camera.getTarget(), glm::vec3(5.0f, 0.5f, cosine30));
    glm::mat4 expected(0.0f);
    expected[0] = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    expected[1] = glm::vec4(0.0f, cosine30, -0.5f, 0.0f);
    expected[2] = glm::vec4(0.0f, -0.5f, -cosine30, 0.0f);
    expected[3] = glm::vec4(5.0f, 0.0f, 0.0f, 1.0f);
    requireMatrix(camera.getViewMatrix(), expected);
    const float distance = 2.0f * camera.getKeyboardSpeed();
    SECTION("Forward")
    {
        camera.moveForward(2.0f);
        requireVector(camera.getPosition(), glm::vec3(5.0f, 0.5f * distance, cosine30 * distance));
    }
    SECTION("Right")
    {
        camera.moveRight(2.0f);
        requireVector(camera.getPosition(), glm::vec3(5.0f - distance, 0.0f, 0.0f));
    }
    SECTION("Up")
    {
        camera.moveUp(2.0f);
        requireVector(camera.getPosition(), glm::vec3(5.0f, cosine30 * distance, -0.5f * distance));
    }
}

TEST_CASE("Camera pitch limits cover both poles and mouse recovery", "[camera]")
{
    // WHY: Both signs detect one-sided clamps; setFront's distinct limit must preserve its input.
    for (float sign : {-1.0f, 1.0f})
    {
        INFO("pitch sign " << sign);
        Camera camera(glm::vec3(5.0f, 0.0f, 0.0f), 90.0f, 2.0f, 2.0f, 10.0f);
        camera.processMouseInputs(0.0f, sign * 100000.0f);
        const float limit = sign * 89.0f * pi / 180.0f;
        REQUIRE(camera.getPitch() == nearFloat(limit));
        requireVector(camera.getFront(), glm::vec3(-0.0174524064f, sign * 0.9998476952f, 0.0f));
        REQUIRE(glm::length(camera.getFront()) == nearFloat(1.0f));
        camera.processMouseInputs(0.0f, -sign * 200.0f);
        const float recovered = limit - sign * 200.0f * camera.getMouseSpeed();
        REQUIRE(camera.getPitch() == nearFloat(recovered));
        requireVector(camera.getFront(), glm::vec3(-std::cos(recovered), std::sin(recovered), 0.0f));

        const float angle = 89.95f * pi / 180.0f;
        const glm::vec3 supplied(std::cos(angle), sign * std::sin(angle), 0.0f);
        camera.setFront(supplied);
        requireVector(camera.getFront(), supplied);
        REQUIRE(camera.getYaw() == nearFloat(pi / 2.0f));
        REQUIRE(camera.getPitch() == nearFloat(sign * 89.9f * pi / 180.0f));
        camera.processMouseInputs(0.0f, 0.0f);
        REQUIRE(camera.getPitch() == nearFloat(limit));
        requireVector(camera.getFront(), glm::vec3(0.0174524064f, sign * 0.9998476952f, 0.0f));
    }
}

TEST_CASE("Animator uses 1-based top-to-bottom UVs and horizontal flipping", "[animator]")
{
    // WHY: Non-square sheet corners and an interior flipped cell expose indexing and U/V mistakes.
    Animator animator(3, 8);
    REQUIRE(animator.getFrame() == 0);
    REQUIRE(animator.isAnimationFinished());
    // Configure valid indices before UV access; the constructor's zero indices are sentinels.
    animator.setAnimation(1, 8, 1, 4.0f);
    requireVector(animator.updateUV(0.0f, true), glm::vec4(0.0f, 2.0f / 3.0f, 0.125f, 1.0f));
    animator.setAnimation(8, 8, 3, 4.0f);
    requireVector(animator.updateUV(0.0f, true), glm::vec4(0.875f, 0.0f, 1.0f, 1.0f / 3.0f));
    animator.setAnimation(3, 6, 2, 4.0f);
    requireVector(animator.updateUV(0.0f, true), glm::vec4(0.25f, 1.0f / 3.0f, 0.375f, 2.0f / 3.0f));
    requireVector(animator.updateUV(0.0f, false), glm::vec4(0.375f, 1.0f / 3.0f, 0.25f, 2.0f / 3.0f));
    REQUIRE(animator.getFrame() == 3);
    requireVector(animator.updateUV(0.25f, false), glm::vec4(0.5f, 1.0f / 3.0f, 0.375f, 2.0f / 3.0f));
    REQUIRE(animator.getFrame() == 4);
}

TEST_CASE("Animator accumulates partial durations and advances at the exact boundary", "[animator]")
{
    // WHY: Binary-exact durations distinguish accumulated timing from per-call timing.
    Animator animator(4, 8);
    animator.setAnimation(2, 4, 2, 4.0f);
    animator.updateUV(0.0625f, true);
    REQUIRE(animator.getFrame() == 2);
    animator.updateUV(0.125f, true);
    REQUIRE(animator.getFrame() == 2);
    requireVector(animator.updateUV(0.0625f, true), glm::vec4(0.25f, 0.5f, 0.375f, 0.75f));
    REQUIRE(animator.getFrame() == 3);
    animator.updateUV(0.0f, true);
    REQUIRE(animator.getFrame() == 3);
    animator.updateUV(0.25f, true);
    REQUIRE(animator.getFrame() == 4);
    REQUIRE(animator.isAnimationFinished());
}

TEST_CASE("Animation selection resets frame accumulator row and FPS", "[animator]")
{
    // WHY: Both restart and replacement must clear pending time rather than inherit a previous timer.
    Animator animator(3, 8);
    animator.setAnimation(2, 5, 2, 4.0f);
    animator.updateUV(0.25f, true);
    REQUIRE(animator.getFrame() == 3);
    animator.updateUV(0.1875f, true);
    SECTION("Selecting the same animation restarts")
    {
        animator.setAnimation(2, 5, 2, 4.0f);
        REQUIRE(animator.getFrame() == 2);
        requireVector(animator.updateUV(0.125f, true), glm::vec4(0.125f, 1.0f / 3.0f, 0.25f, 2.0f / 3.0f));
        REQUIRE(animator.getFrame() == 2);
        animator.updateUV(0.125f, true);
        REQUIRE(animator.getFrame() == 3);
    }
    SECTION("Changing the animation replaces row range and duration")
    {
        animator.setAnimation(3, 4, 3, 2.0f);
        REQUIRE(animator.getFrame() == 3);
        REQUIRE_FALSE(animator.isAnimationFinished());
        requireVector(animator.updateUV(0.25f, true), glm::vec4(0.25f, 0.0f, 0.375f, 1.0f / 3.0f));
        REQUIRE(animator.getFrame() == 3);
        animator.updateUV(0.25f, true);
        REQUIRE(animator.getFrame() == 4);
        REQUIRE(animator.isAnimationFinished());
        animator.updateUV(0.5f, true);
        REQUIRE(animator.getFrame() == 3);
        REQUIRE_FALSE(animator.isAnimationFinished());
    }
}

TEST_CASE("Animator end detection is transient and loops to the configured start", "[animator]")
{
    // WHY: Completion compares the current frame and must not latch or stop playback at the end.
    Animator animator(4, 8);
    animator.setAnimation(3, 5, 4, 4.0f);
    REQUIRE_FALSE(animator.isAnimationFinished());
    animator.updateUV(0.25f, true);
    REQUIRE(animator.getFrame() == 4);
    REQUIRE_FALSE(animator.isAnimationFinished());
    requireVector(animator.updateUV(0.25f, true), glm::vec4(0.5f, 0.0f, 0.625f, 0.25f));
    REQUIRE(animator.getFrame() == 5);
    REQUIRE(animator.isAnimationFinished());
    animator.updateUV(0.0f, true);
    REQUIRE(animator.isAnimationFinished());
    requireVector(animator.updateUV(0.25f, true), glm::vec4(0.25f, 0.0f, 0.375f, 0.25f));
    REQUIRE(animator.getFrame() == 3);
    REQUIRE_FALSE(animator.isAnimationFinished());
}

TEST_CASE("Single-frame animation remains on its cell while looping", "[animator]")
{
    // WHY: start==end is valid and must not expose an adjacent column after timed updates.
    Animator animator(4, 8);
    animator.setAnimation(5, 5, 4, 8.0f);
    for (float dt : {0.0f, 0.125f, 10.0f})
    {
        INFO("single-frame dt " << dt);
        requireVector(animator.updateUV(dt, true), glm::vec4(0.5f, 0.0f, 0.625f, 0.25f));
        REQUIRE(animator.getFrame() == 5);
        REQUIRE(animator.isAnimationFinished());
    }
}

TEST_CASE("Large animator dt advances once and discards excess time", "[animator]")
{
    // WHY: Preserve legacy frame-rate-dependent timing instead of accidentally requiring catch-up.
    Animator animator(4, 8);
    animator.setAnimation(2, 4, 2, 4.0f);
    requireVector(animator.updateUV(3.125f, true), glm::vec4(0.25f, 0.5f, 0.375f, 0.75f));
    REQUIRE(animator.getFrame() == 3);
    animator.updateUV(0.0f, true);
    REQUIRE(animator.getFrame() == 3);
    animator.updateUV(0.125f, true);
    REQUIRE(animator.getFrame() == 3);
    animator.updateUV(0.125f, true);
    REQUIRE(animator.getFrame() == 4);
    REQUIRE(animator.isAnimationFinished());
    animator.updateUV(20.0f, true);
    REQUIRE(animator.getFrame() == 2);
    REQUIRE_FALSE(animator.isAnimationFinished());
}

TEST_CASE("Game object constructor and position size setters preserve independent properties", "[game_obj]")
{
    // WHY: Distinct values expose axis and constructor mixups without relying on default zeroes.
    GameObj object(glm::vec2(10.0f, -4.0f), glm::vec2(6.0f, 9.0f), glm::vec2(8.0f, -3.0f));
    requireVector(object.getPosition(), glm::vec2(10.0f, -4.0f));
    REQUIRE(object.getHorizontalPosition() == nearFloat(10.0f));
    REQUIRE(object.getVerticalPosition() == nearFloat(-4.0f));
    requireVector(object.getSize(), glm::vec2(6.0f, 9.0f));
    requireVector(object.getVelocity(), glm::vec2(8.0f, -3.0f));
    REQUIRE(object.getHorizontalVelocity() == nearFloat(8.0f));
    REQUIRE(object.getVerticalVelocity() == nearFloat(-3.0f));
    object.setPosition(glm::vec2(-7.0f, 12.0f));
    requireVector(object.getPosition(), glm::vec2(-7.0f, 12.0f));
    object.setHorizontalPosition(3.5f);
    requireVector(object.getPosition(), glm::vec2(3.5f, 12.0f));
    REQUIRE(object.getHorizontalPosition() == nearFloat(3.5f));
    object.setVerticalPosition(-2.5f);
    requireVector(object.getPosition(), glm::vec2(3.5f, -2.5f));
    REQUIRE(object.getVerticalPosition() == nearFloat(-2.5f));
    requireVector(object.getSize(), glm::vec2(6.0f, 9.0f));
    object.setSize(glm::vec2(2.5f, 7.5f));
    requireVector(object.getSize(), glm::vec2(2.5f, 7.5f));
    requireVector(object.getPosition(), glm::vec2(3.5f, -2.5f));
    requireVector(object.getVelocity(), glm::vec2(8.0f, -3.0f));
}

TEST_CASE("Game object follows an analytic constant-velocity trajectory", "[game_obj]")
{
    // WHY: Unequal binary-exact time steps reveal missing dt factors and unintended acceleration.
    GameObj object(glm::vec2(10.0f, -4.0f), glm::vec2(6.0f, 9.0f), glm::vec2(8.0f, 3.0f));
    const std::array<float, 4> timeSteps{{0.125f, 0.25f, 0.625f, 2.0f}};
    const std::array<glm::vec2, 4> positions{{
        glm::vec2(11.0f, -3.625f), glm::vec2(13.0f, -2.875f),
        glm::vec2(18.0f, -1.0f), glm::vec2(34.0f, 5.0f)
    }};
    for (std::size_t step = 0; step < timeSteps.size(); ++step)
    {
        INFO("trajectory step " << step);
        object.updatePosition(timeSteps[step]);
        requireVector(object.getPosition(), positions[step]);
        REQUIRE(object.getHorizontalPosition() == nearFloat(positions[step].x));
        REQUIRE(object.getVerticalPosition() == nearFloat(positions[step].y));
        requireVector(object.getVelocity(), glm::vec2(8.0f, 3.0f));
        requireVector(object.getSize(), glm::vec2(6.0f, 9.0f));
    }
}

TEST_CASE("Zero time zero velocity and signed time preserve game object integration rules", "[game_obj]")
{
    // WHY: No-op and backward integration cases detect clamping and accidental property mutation.
    GameObj object(glm::vec2(3.0f, 4.0f), glm::vec2(6.0f, 9.0f), glm::vec2(8.0f, -6.0f));
    object.updatePosition(0.0f);
    requireVector(object.getPosition(), glm::vec2(3.0f, 4.0f));
    object.updatePosition(-0.5f);
    requireVector(object.getPosition(), glm::vec2(-1.0f, 7.0f));
    object.updatePosition(0.5f);
    requireVector(object.getPosition(), glm::vec2(3.0f, 4.0f));
    requireVector(object.getVelocity(), glm::vec2(8.0f, -6.0f));
    object.setVelocity(glm::vec2(0.0f, 0.0f));
    object.updatePosition(0.5f);
    object.updatePosition(100.0f);
    requireVector(object.getPosition(), glm::vec2(3.0f, 4.0f));
    requireVector(object.getVelocity(), glm::vec2(0.0f, 0.0f));
    requireVector(object.getSize(), glm::vec2(6.0f, 9.0f));
}

TEST_CASE("Velocity setters change subsequent motion independently including negative components", "[game_obj]")
{
    // WHY: Piecewise analytic motion catches stale velocities and scalar setter axis coupling.
    GameObj object(glm::vec2(-2.0f, 9.0f), glm::vec2(6.0f, 9.0f), glm::vec2(-4.0f, -6.0f));
    object.updatePosition(1.5f);
    requireVector(object.getPosition(), glm::vec2(-8.0f, 0.0f));
    object.setHorizontalVelocity(8.0f);
    requireVector(object.getVelocity(), glm::vec2(8.0f, -6.0f));
    REQUIRE(object.getHorizontalVelocity() == nearFloat(8.0f));
    REQUIRE(object.getVerticalVelocity() == nearFloat(-6.0f));
    requireVector(object.getPosition(), glm::vec2(-8.0f, 0.0f));
    object.updatePosition(0.25f);
    requireVector(object.getPosition(), glm::vec2(-6.0f, -1.5f));
    object.setVerticalVelocity(0.0f);
    requireVector(object.getVelocity(), glm::vec2(8.0f, 0.0f));
    REQUIRE(object.getHorizontalVelocity() == nearFloat(8.0f));
    REQUIRE(object.getVerticalVelocity() == nearFloat(0.0f));
    object.updatePosition(0.5f);
    requireVector(object.getPosition(), glm::vec2(-2.0f, -1.5f));
    object.setVelocity(glm::vec2(0.0f, 10.0f));
    requireVector(object.getVelocity(), glm::vec2(0.0f, 10.0f));
    REQUIRE(object.getHorizontalVelocity() == nearFloat(0.0f));
    REQUIRE(object.getVerticalVelocity() == nearFloat(10.0f));
    object.updatePosition(0.125f);
    requireVector(object.getPosition(), glm::vec2(-2.0f, -0.25f));
    requireVector(object.getSize(), glm::vec2(6.0f, 9.0f));
}

TEST_CASE("State names cover every enumerator and valid unnamed values", "[state]")
{
    // WHY: Exhaustive string-content checks catch omissions and avoid comparing literal addresses.
    struct NamedState { State state; std::string_view name; };
    const std::array<NamedState, 6> states{{
        {State::Idle, "IDLE"}, {State::Walk, "WALK"}, {State::Jump, "JUMP"},
        {State::Attack, "ATTACK"}, {State::Hit, "HIT"}, {State::Dead, "DEAD"}
    }};
    for (const NamedState& state : states)
    {
        INFO("named state " << state.name);
        const char* actual = stateToString(state.state);
        REQUIRE(actual != nullptr);
        REQUIRE(std::string_view(actual) == state.name);
    }
    // Scoped State has a fixed underlying int type; these unnamed values are valid enum objects.
    for (int value : {-1, 6, 99})
    {
        INFO("unnamed underlying value " << value);
        const char* actual = stateToString(static_cast<State>(value));
        REQUIRE(actual != nullptr);
        REQUIRE(std::string_view(actual) == "UNKNOWN");
    }
}
