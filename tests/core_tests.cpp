#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "test_support.h"
#include "glfw_input_fake.h"
#include "core/action_map.h"
#include "core/collision.h"
#include "core/geometry.h"
#include "core/input_manager.h"
#include "graphics/vertex.h"
#include "app/scene3D/scene_data.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>

namespace
{
using test_support::requireVec2;
using test_support::requireVec3;

Vertex vertexAt(const glm::vec3& position)
{
    return Vertex(position, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f));
}

/** @brief Own the vertices for as long as TriangleData retains its reference. */
struct UnitTriangle
{
    std::vector<Vertex> vertices{
        vertexAt({0.0f, 0.0f, 0.0f}),
        vertexAt({1.0f, 0.0f, 0.0f}),
        vertexAt({0.0f, 1.0f, 0.0f})};
    TriangleData triangle{0, 1, 2, vertices};
};

/** @brief Isolate fake readings and preserve ActionMap's reference lifetime. */
struct InputFixture
{
    glfw_input_fake::ScopedState fake;
    InputManager input;
    ActionMap actions{input};
};

/** @brief Check exclusivity as well as the requested state at every transition. */
void requireInputState(const InputManager& input, int code, KeyState expected)
{
    CAPTURE(code);
    REQUIRE(input.isPressed(code) == (expected == KeyState::Pressed));
    REQUIRE(input.isHeld(code) == (expected == KeyState::Held));
    REQUIRE(input.isReleased(code) == (expected == KeyState::Released));
}

void requireInactiveAction(const ActionMap& actions, Action action)
{
    REQUIRE_FALSE(actions.justStarted(action));
    REQUIRE_FALSE(actions.ongoing(action));
    REQUIRE_FALSE(actions.justFinished(action));
}
} // namespace

/** @test GPU upload depends on attribute layout and constructor values. */
TEST_CASE("Vertex stores signed position normal and texture coordinates", "[vertex]")
{
    STATIC_REQUIRE(std::is_standard_layout_v<Vertex>);
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<Vertex>);
    STATIC_REQUIRE(offsetof(Vertex, m_position) == 0);
    STATIC_REQUIRE(offsetof(Vertex, m_normal) > offsetof(Vertex, m_position));
    STATIC_REQUIRE(offsetof(Vertex, m_texCoords) > offsetof(Vertex, m_normal));

    const Vertex vertex({-2.5f, 0.0f, 3.25f}, {0.25f, -0.5f, 0.75f},
                        {-0.25f, 1.5f});
    requireVec3(vertex.getPosition(), {-2.5f, 0.0f, 3.25f});
    requireVec3(vertex.getNormal(), {0.25f, -0.5f, 0.75f});
    requireVec2(vertex.getTexCoords(), {-0.25f, 1.5f});
}

TEST_CASE("Vertex getters reference the stored attributes", "[vertex]")
{
    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const Vertex&>().getPosition()),
                                 const glm::vec3&>);
    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const Vertex&>().getNormal()),
                                 const glm::vec3&>);
    STATIC_REQUIRE(std::is_same_v<decltype(std::declval<const Vertex&>().getTexCoords()),
                                 const glm::vec2&>);
    Vertex vertex({1.0f, 2.0f, 3.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f});
    const glm::vec3& position = vertex.getPosition();
    const glm::vec3& normal = vertex.getNormal();
    const glm::vec2& uv = vertex.getTexCoords();
    REQUIRE(std::addressof(position) == std::addressof(vertex.m_position));
    REQUIRE(std::addressof(normal) == std::addressof(vertex.m_normal));
    REQUIRE(std::addressof(uv) == std::addressof(vertex.m_texCoords));

    vertex.m_position = {-4.0f, 5.0f, -6.0f};
    vertex.m_normal = {1.0f, 0.0f, 0.0f};
    vertex.m_texCoords = {0.25f, 0.75f};
    requireVec3(position, {-4.0f, 5.0f, -6.0f});
    requireVec3(normal, {1.0f, 0.0f, 0.0f});
    requireVec2(uv, {0.25f, 0.75f});
}

TEST_CASE("Copying a vertex does not alias its attributes", "[vertex]")
{
    const Vertex original({-1.0f, 2.0f, 3.0f}, {0.0f, 0.0f, -1.0f}, {0.5f, 0.25f});
    Vertex copy = original;
    copy.m_position.x = 9.0f;
    copy.m_normal.z = 1.0f;
    copy.m_texCoords.y = 0.75f;
    requireVec3(original.getPosition(), {-1.0f, 2.0f, 3.0f});
    requireVec3(original.getNormal(), {0.0f, 0.0f, -1.0f});
    requireVec2(original.getTexCoords(), {0.5f, 0.25f});
}

/** @test Nonzero indices expose constructors that accidentally assume vertices 0..2. */
TEST_CASE("TriangleData selects mesh indices and caches edges and a unit normal", "[geometry]")
{
    const std::vector<Vertex> vertices{
        vertexAt({100.0f, 100.0f, 100.0f}),
        vertexAt({1.0f, -2.0f, 3.0f}),
        vertexAt({3.0f, -2.0f, 3.0f}),
        vertexAt({1.0f, 1.0f, 3.0f})};
    const TriangleData triangle(1, 2, 3, vertices);
    REQUIRE(triangle.m_iA == 1);
    REQUIRE(triangle.m_iB == 2);
    REQUIRE(triangle.m_iC == 3);
    REQUIRE(std::addressof(triangle.m_vA) == std::addressof(vertices[1].m_position));
    requireVec3(triangle.m_vA, {1.0f, -2.0f, 3.0f});
    requireVec3(triangle.m_e1, {2.0f, 0.0f, 0.0f});
    requireVec3(triangle.m_e2, {0.0f, 3.0f, 0.0f});
    requireVec3(triangle.m_normal, {0.0f, 0.0f, 1.0f});
    REQUIRE(glm::length(triangle.m_normal) == Catch::Approx(1.0f));
}

TEST_CASE("Reversing triangle winding reverses its normal", "[geometry]")
{
    const UnitTriangle fixture;
    const TriangleData reversed(0, 2, 1, fixture.vertices);
    requireVec3(reversed.m_normal, {0.0f, 0.0f, -1.0f});
    requireVec3(reversed.m_e1, fixture.triangle.m_e2);
    requireVec3(reversed.m_e2, fixture.triangle.m_e1);
}

/** @test Ray parameters must survive nonuniform object-space transforms unchanged. */
TEST_CASE("Ray preserves origin and an unnormalized or zero direction", "[geometry]")
{
    const Ray ray({-3.5f, 2.25f, -7.0f}, {0.0f, -4.0f, 2.0f});
    requireVec3(ray.m_origin, {-3.5f, 2.25f, -7.0f});
    requireVec3(ray.m_direction, {0.0f, -4.0f, 2.0f});
    const Ray stationary({1.0f, -1.0f, 0.0f}, glm::vec3(0.0f));
    requireVec3(stationary.m_direction, glm::vec3(0.0f));
}

TEST_CASE("Ray intersection returns a parameter rather than a normalized distance", "[geometry]")
{
    const UnitTriangle fixture;
    const Ray ray({0.25f, 0.25f, 6.0f}, {0.0f, 0.0f, -2.0f});
    float parameter = -99.0f;
    REQUIRE(mollerTrumbore(ray, fixture.triangle, parameter));
    REQUIRE(parameter == Catch::Approx(3.0f));
    requireVec3(ray.m_origin + parameter * ray.m_direction, {0.25f, 0.25f, 0.0f});
}

TEST_CASE("Ray intersection accepts both triangle windings and both faces", "[geometry]")
{
    const UnitTriangle fixture;
    const TriangleData reversed(0, 2, 1, fixture.vertices);
    for (const TriangleData* triangle : {&fixture.triangle, &reversed})
    {
        for (float side : {-1.0f, 1.0f})
        {
            CAPTURE(side, triangle->m_iB);
            float parameter = -1.0f;
            const Ray ray({0.25f, 0.25f, side * 2.0f}, {0.0f, 0.0f, -side});
            REQUIRE(mollerTrumbore(ray, *triangle, parameter));
            REQUIRE(parameter == Catch::Approx(2.0f));
        }
    }
}

TEST_CASE("Ray intersection includes all triangle vertices and edges", "[geometry][boundary]")
{
    const UnitTriangle fixture;
    const std::array<glm::vec2, 7> points{{
        {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f},
        {0.5f, 0.0f}, {0.0f, 0.5f}, {0.5f, 0.5f}, {0.25f, 0.25f}}};
    for (const auto& point : points)
    {
        CAPTURE(point.x, point.y);
        float parameter = -1.0f;
        REQUIRE(mollerTrumbore(Ray({point.x, point.y, 2.0f}, {0.0f, 0.0f, -1.0f}),
                              fixture.triangle, parameter));
        REQUIRE(parameter == Catch::Approx(2.0f));
    }
}

TEST_CASE("Ray intersection accepts barycentric points within the configured epsilon",
          "[geometry][boundary]")
{
    const UnitTriangle fixture;
    const float epsilon = sceneData::intersectionEpsilon;
    const std::array<glm::vec2, 4> points{{
        {-0.5f * epsilon, 0.25f}, {0.25f, -0.5f * epsilon},
        {1.0f + 0.5f * epsilon, 0.0f}, {0.5f, 0.5f + 0.5f * epsilon}}};
    for (const auto& point : points)
    {
        CAPTURE(point.x, point.y);
        float parameter = -1.0f;
        REQUIRE(mollerTrumbore(Ray({point.x, point.y, 1.0f}, {0.0f, 0.0f, -1.0f}),
                              fixture.triangle, parameter));
        REQUIRE(parameter == Catch::Approx(1.0f));
    }
}

/** @test Each negative sample exercises a different rejection and preserves the output. */
TEST_CASE("Ray intersection rejects points beyond each triangle edge without changing output",
          "[geometry][negative]")
{
    const UnitTriangle fixture;
    const float epsilon = sceneData::intersectionEpsilon;
    const std::array<glm::vec2, 6> points{{
        {-4.0f * epsilon, 0.25f}, {0.25f, -4.0f * epsilon},
        {1.0f + 4.0f * epsilon, 0.0f}, {0.5f, 0.5f + 4.0f * epsilon},
        {-1.0f, -1.0f}, {0.75f, 0.75f}}};
    for (const auto& point : points)
    {
        CAPTURE(point.x, point.y);
        float parameter = 123.25f;
        REQUIRE_FALSE(mollerTrumbore(
            Ray({point.x, point.y, 1.0f}, {0.0f, 0.0f, -1.0f}),
            fixture.triangle, parameter));
        REQUIRE(parameter == Catch::Approx(123.25f));
    }
}

TEST_CASE("Ray intersection rejects parallel coplanar and stationary rays", "[geometry][negative]")
{
    const UnitTriangle fixture;
    const std::array<Ray, 3> rays{{
        Ray({0.25f, 0.25f, 1.0f}, {1.0f, 0.0f, 0.0f}),
        Ray({0.25f, 0.25f, 0.0f}, {0.0f, 1.0f, 0.0f}),
        Ray({0.25f, 0.25f, 1.0f}, {0.0f, 0.0f, 0.0f})}};
    for (const auto& ray : rays)
    {
        CAPTURE(ray.m_origin.z, ray.m_direction.x, ray.m_direction.y);
        float parameter = 123.25f;
        REQUIRE_FALSE(mollerTrumbore(ray, fixture.triangle, parameter));
        REQUIRE(parameter == Catch::Approx(123.25f));
    }
}

TEST_CASE("Ray intersection rejects a triangle behind the ray origin", "[geometry][negative]")
{
    const UnitTriangle fixture;
    float parameter = 123.25f;
    REQUIRE_FALSE(mollerTrumbore(
        Ray({0.25f, 0.25f, -1.0f}, {0.0f, 0.0f, -1.0f}),
        fixture.triangle, parameter));
    REQUIRE(parameter == Catch::Approx(123.25f));
}

/** @test The legacy epsilon explicitly admits tiny negative ray parameters. */
TEST_CASE("Ray intersection includes zero and negative parameters within epsilon",
          "[geometry][boundary]")
{
    const UnitTriangle fixture;
    const float epsilon = sceneData::intersectionEpsilon;
    for (float height : {0.0f, -0.5f * epsilon, -epsilon})
    {
        CAPTURE(height);
        float parameter = 123.25f;
        REQUIRE(mollerTrumbore(Ray({0.25f, 0.25f, height}, {0.0f, 0.0f, -1.0f}),
                              fixture.triangle, parameter));
        REQUIRE(parameter == Catch::Approx(height).epsilon(0).margin(epsilon * 0.01f));
    }
    float parameter = 123.25f;
    REQUIRE_FALSE(mollerTrumbore(
        Ray({0.25f, 0.25f, -2.0f * epsilon}, {0.0f, 0.0f, -1.0f}),
        fixture.triangle, parameter));
    REQUIRE(parameter == Catch::Approx(123.25f));
}

TEST_CASE("Ray intersection rejects a determinant below the epsilon threshold",
          "[geometry][boundary]")
{
    const UnitTriangle fixture;
    float parameter = 123.25f;
    REQUIRE_FALSE(mollerTrumbore(
        Ray({0.25f, 0.25f, 1.0f}, {0.0f, 0.0f, -0.5f * sceneData::intersectionEpsilon}),
        fixture.triangle, parameter));
    REQUIRE(parameter == Catch::Approx(123.25f));
}

TEST_CASE("Ray intersection accepts either determinant sign at the epsilon threshold",
          "[geometry][boundary]")
{
    const UnitTriangle fixture;
    const float epsilon = sceneData::intersectionEpsilon;
    for (float side : {-1.0f, 1.0f})
    {
        CAPTURE(side);
        float parameter = -1.0f;
        REQUIRE(mollerTrumbore(
            Ray({0.25f, 0.25f, side}, {0.0f, 0.0f, -side * epsilon}),
            fixture.triangle, parameter));
        REQUIRE(parameter == Catch::Approx(1.0f / epsilon).epsilon(1e-5f));
    }
}

TEST_CASE("Ray intersection rejects a collinear triangle", "[geometry][negative]")
{
    const std::vector<Vertex> vertices{
        vertexAt({-1.0f, 0.0f, 0.0f}),
        vertexAt({0.0f, 0.0f, 0.0f}),
        vertexAt({1.0f, 0.0f, 0.0f})};
    const TriangleData triangle(0, 1, 2, vertices);
    float parameter = 123.25f;
    REQUIRE_FALSE(mollerTrumbore(Ray({0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}),
                                triangle, parameter));
    REQUIRE(parameter == Catch::Approx(123.25f));
}

/**
 * @test Construct rays from known points and parameters on different planes.
 * @details Fixed barycentric samples include vertices, edges, and interiors.
 * This supplies an independent geometric oracle without copying the
 * Moller-Trumbore algorithm or relying on nondeterministic random input.
 */
TEST_CASE("Deterministic ray samples recover known points on translated and tilted triangles",
          "[geometry][property]")
{
    struct Sample
    {
        glm::vec3 a, b, c, normal;
    };
    const std::array<Sample, 4> samples{{
        {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-3.0f, -4.0f, -2.0f}, {-1.0f, -4.0f, -2.0f}, {-3.0f, -1.0f, -2.0f}, {0.0f, 0.0f, 1.0f}},
        {{-2.0f, -1.0f, -3.0f}, {-2.0f, 1.0f, -3.0f}, {-2.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{-2.0f, 1.0f, -3.0f}, {0.0f, 1.0f, -1.0f}, {-2.0f, 4.0f, -3.0f}, {-1.0f, 0.0f, 1.0f}}
    }};
    const std::array<glm::vec2, 7> barycentrics{{
        {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {0.5f, 0.0f},
        {0.0f, 0.5f}, {0.5f, 0.5f}, {0.25f, 0.5f}}};
    for (std::size_t index = 0; index < samples.size(); ++index)
    {
        const auto& sample = samples[index];
        const std::vector<Vertex> vertices{
            vertexAt(sample.a), vertexAt(sample.b), vertexAt(sample.c)};
        const TriangleData triangle(0, 1, 2, vertices);
        for (const auto& weights : barycentrics)
        {
            const glm::vec3 point = (1.0f - weights.x - weights.y) * sample.a +
                                    weights.x * sample.b + weights.y * sample.c;
            for (float speed : {-2.0f, 0.5f})
            {
                for (float expectedParameter : {0.0f, 0.25f, 3.0f})
                {
                    CAPTURE(index, weights.x, weights.y, speed, expectedParameter);
                    const glm::vec3 direction = speed * sample.normal;
                    const Ray ray(point - expectedParameter * direction, direction);
                    float parameter = -99.0f;
                    REQUIRE(mollerTrumbore(ray, triangle, parameter));
                    REQUIRE(parameter ==
                            Catch::Approx(expectedParameter).epsilon(0).margin(2e-5f));
                    requireVec3(ray.m_origin + parameter * ray.m_direction, point, 5e-5f);
                }
            }
        }
    }
}

TEST_CASE("Deterministic rays outside a tilted triangle remain misses under translation",
          "[geometry][property][negative]")
{
    const std::array<glm::vec3, 3> translations{{
        {0.0f, 0.0f, 0.0f}, {-8.0f, 4.0f, -2.0f}, {16.0f, -8.0f, 3.0f}}};
    const std::array<glm::vec2, 5> barycentrics{{
        {-0.25f, 0.5f}, {0.5f, -0.25f}, {0.75f, 0.75f}, {1.5f, 0.0f}, {0.0f, 1.5f}}};
    for (const auto& translation : translations)
    {
        const glm::vec3 a = glm::vec3(-2.0f, 1.0f, -3.0f) + translation;
        const glm::vec3 b = glm::vec3(0.0f, 1.0f, -1.0f) + translation;
        const glm::vec3 c = glm::vec3(-2.0f, 4.0f, -3.0f) + translation;
        const std::vector<Vertex> vertices{vertexAt(a), vertexAt(b), vertexAt(c)};
        const TriangleData triangle(0, 1, 2, vertices);
        for (const auto& weights : barycentrics)
        {
            CAPTURE(translation.x, translation.y, translation.z, weights.x, weights.y);
            const glm::vec3 point = (1.0f - weights.x - weights.y) * a +
                                    weights.x * b + weights.y * c;
            const glm::vec3 direction(-1.0f, 0.0f, 1.0f);
            float parameter = 123.25f;
            REQUIRE_FALSE(mollerTrumbore(Ray(point - 3.0f * direction, direction),
                                        triangle, parameter));
            REQUIRE(parameter == Catch::Approx(123.25f));
        }
    }
}

TEST_CASE("BoundingBox constructor treats position as minimum and preserves size", "[collision]")
{
    const BoundingBox box({-4.0f, 3.0f}, {2.5f, 6.0f});
    requireVec2(box.m_min, {-4.0f, 3.0f});
    requireVec2(box.m_max, {-1.5f, 9.0f});
    requireVec2(box.m_size, {2.5f, 6.0f});
}

TEST_CASE("Moving a BoundingBox updates both corners without accumulating displacement",
          "[collision]")
{
    BoundingBox box({-4.0f, 3.0f}, {2.5f, 6.0f});
    box.updateBB({8.0f, -9.0f});
    requireVec2(box.m_min, {8.0f, -9.0f});
    requireVec2(box.m_max, {10.5f, -3.0f});
    box.updateBB({-2.0f, -3.0f});
    requireVec2(box.m_min, {-2.0f, -3.0f});
    requireVec2(box.m_max, {0.5f, 3.0f});
    requireVec2(box.m_size, {2.5f, 6.0f});
}

TEST_CASE("BoundingBox storage preserves signed extents without normalizing corners", "[collision]")
{
    BoundingBox box({1.0f, -2.0f}, {-3.0f, 0.0f});
    requireVec2(box.m_size, {-3.0f, 0.0f});
    requireVec2(box.m_max, {-2.0f, -2.0f});
    box.updateBB({-5.0f, 4.0f});
    requireVec2(box.m_min, {-5.0f, 4.0f});
    requireVec2(box.m_max, {-8.0f, 4.0f});
}

TEST_CASE("Bounding boxes intersect for positive overlap containment and identical boxes",
          "[collision]")
{
    const BoundingBox box({-2.0f, -2.0f}, {4.0f, 4.0f});
    const std::array<BoundingBox, 3> overlaps{{
        BoundingBox({1.0f, -1.0f}, {3.0f, 2.0f}),
        BoundingBox({-1.0f, -1.0f}, {1.0f, 1.0f}),
        BoundingBox({-2.0f, -2.0f}, {4.0f, 4.0f})}};
    for (const auto& other : overlaps)
    {
        CAPTURE(other.m_min.x, other.m_min.y);
        REQUIRE(intersectBB(box, other));
        REQUIRE(intersectBB(other, box));
    }
}

/** @test Strict box comparisons distinguish collision from mere edge contact. */
TEST_CASE("Bounding boxes touching at any edge or corner do not intersect",
          "[collision][boundary]")
{
    const BoundingBox box({-1.0f, -1.0f}, {2.0f, 2.0f});
    const std::array<glm::vec2, 8> positions{{
        {-3.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, -3.0f}, {-1.0f, 1.0f},
        {-3.0f, -3.0f}, {-3.0f, 1.0f}, {1.0f, -3.0f}, {1.0f, 1.0f}}};
    for (const auto& position : positions)
    {
        CAPTURE(position.x, position.y);
        const BoundingBox other(position, {2.0f, 2.0f});
        REQUIRE_FALSE(intersectBB(box, other));
        REQUIRE_FALSE(intersectBB(other, box));
    }
}

TEST_CASE("Bounding boxes separated on either axis do not intersect", "[collision][negative]")
{
    const BoundingBox box({-1.0f, -1.0f}, {2.0f, 2.0f});
    const std::array<glm::vec2, 5> positions{{
        {-4.0f, -1.0f}, {2.0f, -1.0f}, {-1.0f, -4.0f}, {-1.0f, 2.0f}, {4.0f, -4.0f}}};
    for (const auto& position : positions)
    {
        CAPTURE(position.x, position.y);
        const BoundingBox other(position, {2.0f, 2.0f});
        REQUIRE_FALSE(intersectBB(box, other));
        REQUIRE_FALSE(intersectBB(other, box));
    }
}

TEST_CASE("Bounding boxes distinguish a small overlap from a small gap", "[collision][boundary]")
{
    const BoundingBox box({0.0f, 0.0f}, {1.0f, 1.0f});
    // A binary-exact offset keeps this test independent of decimal rounding.
    constexpr float offset = 1.0f / 1024.0f;
    REQUIRE(intersectBB(box, BoundingBox({1.0f - offset, 0.0f}, {1.0f, 1.0f})));
    REQUIRE_FALSE(intersectBB(box, BoundingBox({1.0f + offset, 0.0f}, {1.0f, 1.0f})));
    REQUIRE(intersectBB(box, BoundingBox({0.0f, 1.0f - offset}, {1.0f, 1.0f})));
    REQUIRE_FALSE(intersectBB(box, BoundingBox({0.0f, 1.0f + offset}, {1.0f, 1.0f})));
}

/** @test Known outcomes provide a translation/symmetry oracle, not a copied predicate. */
TEST_CASE("Deterministic box samples preserve intersections under common translation",
          "[collision][property]")
{
    struct Sample { glm::vec2 position, size; bool intersects; };
    const std::array<Sample, 7> samples{{
        {{0.5f, 0.5f}, {2.0f, 2.0f}, true},
        {{0.25f, 0.25f}, {0.5f, 0.5f}, true},
        {{-2.0f, -2.0f}, {5.0f, 5.0f}, true},
        {{2.0f, 0.0f}, {1.0f, 1.0f}, false},
        {{-1.0f, 0.0f}, {1.0f, 1.0f}, false},
        {{0.0f, 2.0f}, {1.0f, 1.0f}, false},
        {{-3.0f, -3.0f}, {1.0f, 1.0f}, false}
    }};
    const std::array<glm::vec2, 4> translations{{
        {0.0f, 0.0f}, {-16.0f, -8.0f}, {32.0f, -4.0f}, {-0.5f, 0.25f}}};
    for (const auto& translation : translations)
    {
        const BoundingBox box(translation, {2.0f, 2.0f});
        for (const auto& sample : samples)
        {
            CAPTURE(translation.x, translation.y, sample.position.x, sample.position.y);
            const BoundingBox other(sample.position + translation, sample.size);
            REQUIRE(intersectBB(box, other) == sample.intersects);
            REQUIRE(intersectBB(other, box) == sample.intersects);
        }
    }
}

TEST_CASE_METHOD(InputFixture, "Registering a keyboard key starts it released without polling",
                 "[input]")
{
    input.registerKey(GLFW_KEY_W);
    requireInputState(input, GLFW_KEY_W, KeyState::Released);
    REQUIRE(glfw_input_fake::keyPolls().empty());
    REQUIRE(glfw_input_fake::mouseButtonPolls().empty());
}

TEST_CASE_METHOD(InputFixture, "Unregistered controls report no state and are never polled",
                 "[input][negative]")
{
    glfw_input_fake::setKey(GLFW_KEY_A, GLFW_PRESS);
    glfw_input_fake::setMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    input.update(nullptr);
    for (int code : {GLFW_KEY_A, GLFW_MOUSE_BUTTON_LEFT, GLFW_KEY_UNKNOWN})
    {
        CAPTURE(code);
        REQUIRE_FALSE(input.isPressed(code));
        REQUIRE_FALSE(input.isHeld(code));
        REQUIRE_FALSE(input.isReleased(code));
    }
    REQUIRE(glfw_input_fake::keyPolls().empty());
    REQUIRE(glfw_input_fake::mouseButtonPolls().empty());
}

TEST_CASE_METHOD(InputFixture, "Keyboard press becomes held then released and can be pressed again",
                 "[input]")
{
    input.registerKey(GLFW_KEY_W);
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_PRESS);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_W, KeyState::Pressed);
    for (int frame = 0; frame < 3; ++frame)
    {
        CAPTURE(frame);
        input.update(nullptr);
        requireInputState(input, GLFW_KEY_W, KeyState::Held);
    }
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_RELEASE);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_W, KeyState::Released);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_W, KeyState::Released);
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_PRESS);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_W, KeyState::Pressed);
}

TEST_CASE_METHOD(InputFixture, "A keyboard tap may release without ever becoming held", "[input]")
{
    input.registerKey(GLFW_KEY_SPACE);
    glfw_input_fake::setKey(GLFW_KEY_SPACE, GLFW_PRESS);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_SPACE, KeyState::Pressed);
    glfw_input_fake::setKey(GLFW_KEY_SPACE, GLFW_RELEASE);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_SPACE, KeyState::Released);
}

TEST_CASE_METHOD(InputFixture, "Mouse buttons follow the same press held release transitions",
                 "[input][mouse]")
{
    input.registerKey(GLFW_MOUSE_BUTTON_RIGHT);
    requireInputState(input, GLFW_MOUSE_BUTTON_RIGHT, KeyState::Released);
    glfw_input_fake::setMouseButton(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);
    input.update(nullptr);
    requireInputState(input, GLFW_MOUSE_BUTTON_RIGHT, KeyState::Pressed);
    input.update(nullptr);
    requireInputState(input, GLFW_MOUSE_BUTTON_RIGHT, KeyState::Held);
    glfw_input_fake::setMouseButton(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE);
    input.update(nullptr);
    requireInputState(input, GLFW_MOUSE_BUTTON_RIGHT, KeyState::Released);
    input.update(nullptr);
    requireInputState(input, GLFW_MOUSE_BUTTON_RIGHT, KeyState::Released);
    glfw_input_fake::setMouseButton(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);
    input.update(nullptr);
    requireInputState(input, GLFW_MOUSE_BUTTON_RIGHT, KeyState::Pressed);
    REQUIRE(glfw_input_fake::keyPolls().empty());
    REQUIRE(glfw_input_fake::mouseButtonPolls().size() == 5);
}

TEST_CASE_METHOD(InputFixture, "Mouse classification includes both endpoints and excludes neighbors",
                 "[input][boundary]")
{
    REQUIRE(input.isMouseButton(GLFW_MOUSE_BUTTON_1));
    REQUIRE(input.isMouseButton(GLFW_MOUSE_BUTTON_LAST));
    REQUIRE_FALSE(input.isMouseButton(GLFW_MOUSE_BUTTON_1 - 1));
    REQUIRE_FALSE(input.isMouseButton(GLFW_MOUSE_BUTTON_LAST + 1));
    REQUIRE_FALSE(input.isMouseButton(GLFW_KEY_UNKNOWN));
    REQUIRE_FALSE(input.isMouseButton(GLFW_KEY_SPACE));
    for (int code = GLFW_MOUSE_BUTTON_1; code <= GLFW_MOUSE_BUTTON_LAST; ++code)
    {
        CAPTURE(code);
        REQUIRE(input.isMouseButton(code));
    }
}

/** @test Separate histories catch keyboard/mouse routing mistakes despite matching states. */
TEST_CASE_METHOD(InputFixture, "Input polls every registered control once through the right API",
                 "[input][mouse]")
{
    // This opaque pointer is an identity token only; the fake never dereferences it.
    int windowToken = 0;
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(&windowToken);
    const std::array<int, 2> keys{{GLFW_KEY_A, GLFW_KEY_Z}};
    const std::array<int, 2> buttons{{GLFW_MOUSE_BUTTON_1, GLFW_MOUSE_BUTTON_LAST}};
    for (int key : keys)
    {
        input.registerKey(key);
        glfw_input_fake::setKey(key, GLFW_PRESS);
    }
    for (int button : buttons)
    {
        input.registerKey(button);
        glfw_input_fake::setMouseButton(button, GLFW_PRESS);
    }
    input.update(window);
    REQUIRE(glfw_input_fake::keyPolls().size() == keys.size());
    REQUIRE(glfw_input_fake::mouseButtonPolls().size() == buttons.size());
    for (int key : keys)
    {
        CAPTURE(key);
        const auto& polls = glfw_input_fake::keyPolls();
        REQUIRE(std::count_if(polls.begin(), polls.end(), [=](const auto& poll) {
            return poll.window == window && poll.code == key;
        }) == 1);
        requireInputState(input, key, KeyState::Pressed);
    }
    for (int button : buttons)
    {
        CAPTURE(button);
        const auto& polls = glfw_input_fake::mouseButtonPolls();
        REQUIRE(std::count_if(polls.begin(), polls.end(), [=](const auto& poll) {
            return poll.window == window && poll.code == button;
        }) == 1);
        requireInputState(input, button, KeyState::Pressed);
    }
}

TEST_CASE_METHOD(InputFixture, "Simultaneous keyboard and mouse controls transition independently",
                 "[input]")
{
    input.registerKey(GLFW_KEY_A);
    input.registerKey(GLFW_KEY_D);
    input.registerKey(GLFW_MOUSE_BUTTON_LEFT);
    glfw_input_fake::setKey(GLFW_KEY_A, GLFW_PRESS);
    glfw_input_fake::setMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_A, KeyState::Pressed);
    requireInputState(input, GLFW_KEY_D, KeyState::Released);
    requireInputState(input, GLFW_MOUSE_BUTTON_LEFT, KeyState::Pressed);

    glfw_input_fake::setKey(GLFW_KEY_A, GLFW_RELEASE);
    glfw_input_fake::setKey(GLFW_KEY_D, GLFW_PRESS);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_A, KeyState::Released);
    requireInputState(input, GLFW_KEY_D, KeyState::Pressed);
    requireInputState(input, GLFW_MOUSE_BUTTON_LEFT, KeyState::Held);
}

TEST_CASE_METHOD(InputFixture, "Registering an existing key resets its legacy state without duplication",
                 "[input]")
{
    input.registerKey(GLFW_KEY_W);
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_PRESS);
    input.update(nullptr);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_W, KeyState::Held);
    input.registerKey(GLFW_KEY_W);
    requireInputState(input, GLFW_KEY_W, KeyState::Released);
    input.update(nullptr);
    requireInputState(input, GLFW_KEY_W, KeyState::Pressed);
    REQUIRE(glfw_input_fake::keyPolls().size() == 3);
}

TEST_CASE_METHOD(InputFixture, "An unbound action reports no state even when controls are active",
                 "[actions][negative]")
{
    input.registerKey(GLFW_KEY_W);
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_PRESS);
    input.update(nullptr);
    requireInactiveAction(actions, Action::MoveForward);
    input.update(nullptr);
    requireInactiveAction(actions, Action::MoveForward);
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_RELEASE);
    input.update(nullptr);
    requireInactiveAction(actions, Action::MoveForward);
}

TEST_CASE_METHOD(InputFixture, "Binding an action does not register or poll its key",
                 "[actions][negative]")
{
    actions.bind(Action::MoveForward, GLFW_KEY_W);
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_PRESS);
    input.update(nullptr);
    requireInactiveAction(actions, Action::MoveForward);
    REQUIRE(glfw_input_fake::keyPolls().empty());
    input.registerKey(GLFW_KEY_W);
    input.update(nullptr);
    REQUIRE(actions.justStarted(Action::MoveForward));
}

TEST_CASE_METHOD(InputFixture, "Action start and ongoing states reflect subsequent input updates",
                 "[actions]")
{
    input.registerKey(GLFW_KEY_W);
    actions.bind(Action::MoveForward, GLFW_KEY_W);
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_PRESS);
    input.update(nullptr);
    REQUIRE(actions.justStarted(Action::MoveForward));
    REQUIRE_FALSE(actions.ongoing(Action::MoveForward));
    REQUIRE_FALSE(actions.justFinished(Action::MoveForward));
    input.update(nullptr);
    REQUIRE_FALSE(actions.justStarted(Action::MoveForward));
    REQUIRE(actions.ongoing(Action::MoveForward));
    REQUIRE_FALSE(actions.justFinished(Action::MoveForward));
    glfw_input_fake::setKey(GLFW_KEY_W, GLFW_RELEASE);
    input.update(nullptr);
    REQUIRE_FALSE(actions.justStarted(Action::MoveForward));
    REQUIRE_FALSE(actions.ongoing(Action::MoveForward));
    REQUIRE(actions.justFinished(Action::MoveForward));
}

/**
 * @test justFinished is a legacy released-state query, not a one-frame edge.
 * @details It is true before the first press and persists while released.
 * Locking down this behavior avoids silently inventing a fourth input state.
 */
TEST_CASE_METHOD(InputFixture, "Action justFinished is true initially and persists while released",
                 "[actions][legacy]")
{
    input.registerKey(GLFW_KEY_SPACE);
    actions.bind(Action::P1Jump, GLFW_KEY_SPACE);
    REQUIRE(actions.justFinished(Action::P1Jump));
    for (int frame = 0; frame < 3; ++frame)
    {
        CAPTURE(frame);
        input.update(nullptr);
        REQUIRE(actions.justFinished(Action::P1Jump));
    }
    glfw_input_fake::setKey(GLFW_KEY_SPACE, GLFW_PRESS);
    input.update(nullptr);
    REQUIRE_FALSE(actions.justFinished(Action::P1Jump));
    glfw_input_fake::setKey(GLFW_KEY_SPACE, GLFW_RELEASE);
    for (int frame = 0; frame < 3; ++frame)
    {
        CAPTURE(frame);
        input.update(nullptr);
        REQUIRE(actions.justFinished(Action::P1Jump));
    }
}

TEST_CASE_METHOD(InputFixture, "Rebinding an action switches its input without affecting other actions",
                 "[actions]")
{
    input.registerKey(GLFW_KEY_A);
    input.registerKey(GLFW_KEY_D);
    actions.bind(Action::P1MoveLeft, GLFW_KEY_A);
    actions.bind(Action::MoveLeft, GLFW_KEY_A);
    glfw_input_fake::setKey(GLFW_KEY_A, GLFW_PRESS);
    input.update(nullptr);
    REQUIRE(actions.justStarted(Action::P1MoveLeft));
    REQUIRE(actions.justStarted(Action::MoveLeft));

    actions.bind(Action::P1MoveLeft, GLFW_KEY_D);
    REQUIRE_FALSE(actions.justStarted(Action::P1MoveLeft));
    REQUIRE(actions.justFinished(Action::P1MoveLeft));
    REQUIRE(actions.justStarted(Action::MoveLeft));
    glfw_input_fake::setKey(GLFW_KEY_D, GLFW_PRESS);
    input.update(nullptr);
    REQUIRE(actions.justStarted(Action::P1MoveLeft));
    REQUIRE(actions.ongoing(Action::MoveLeft));
}

TEST_CASE_METHOD(InputFixture, "Several actions can observe the same key through a shared input manager",
                 "[actions]")
{
    input.registerKey(GLFW_KEY_SPACE);
    actions.bind(Action::P1Jump, GLFW_KEY_SPACE);
    actions.bind(Action::StartAnimation, GLFW_KEY_SPACE);
    glfw_input_fake::setKey(GLFW_KEY_SPACE, GLFW_PRESS);
    input.update(nullptr);
    REQUIRE(actions.justStarted(Action::P1Jump));
    REQUIRE(actions.justStarted(Action::StartAnimation));
    input.update(nullptr);
    REQUIRE(actions.ongoing(Action::P1Jump));
    REQUIRE(actions.ongoing(Action::StartAnimation));
    glfw_input_fake::setKey(GLFW_KEY_SPACE, GLFW_RELEASE);
    input.update(nullptr);
    REQUIRE(actions.justFinished(Action::P1Jump));
    REQUIRE(actions.justFinished(Action::StartAnimation));
}

TEST_CASE_METHOD(InputFixture, "An action bound to a mouse button uses mouse state transitions",
                 "[actions][mouse]")
{
    input.registerKey(GLFW_MOUSE_BUTTON_LEFT);
    actions.bind(Action::SelectObject, GLFW_MOUSE_BUTTON_LEFT);
    glfw_input_fake::setMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
    input.update(nullptr);
    REQUIRE(actions.justStarted(Action::SelectObject));
    REQUIRE_FALSE(actions.ongoing(Action::SelectObject));
    input.update(nullptr);
    REQUIRE(actions.ongoing(Action::SelectObject));
    glfw_input_fake::setMouseButton(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
    input.update(nullptr);
    REQUIRE(actions.justFinished(Action::SelectObject));
    REQUIRE(glfw_input_fake::keyPolls().empty());
}
