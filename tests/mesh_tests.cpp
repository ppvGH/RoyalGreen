#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "test_support.h"
#include "graphics/mesh.h"

namespace
{
Vertex sampleVertex(glm::vec3 position, glm::vec3 normal = {0.0f, 0.0f, 1.0f})
{
    return Vertex(position, normal, {0.0f, 0.0f});
}
}

TEST_CASE("Mesh metadata starts without GPU allocation and stores an explicit shader binding", "[mesh]")
{
    // No setup/draw call is made: metadata and binding must be usable before uploading.
    Mesh mesh;
    REQUIRE(mesh.getVAO() == 0);
    REQUIRE(mesh.m_matIndex == -1);
    REQUIRE(mesh.m_mixTex);
    REQUIRE(mesh.m_UVresize == 0.0f);
    Shader shader; // ID zero means its real destructor does not issue a GL deletion.
    mesh.setShader(shader);
    REQUIRE(mesh.getShader() == &shader);
    mesh.resizeUV(16.0f);
    REQUIRE(mesh.m_UVresize == 16.0f);
}

TEST_CASE("Mesh center uses every vertex including negative coordinates", "[mesh]")
{
    // An asymmetric analytic centroid detects omitted vertices and bounding-box centers.
    Mesh mesh;
    mesh.m_vertices = {sampleVertex({-3.0f, 1.0f, 2.0f}),
                       sampleVertex({6.0f, 4.0f, -1.0f}),
                       sampleVertex({0.0f, -2.0f, 5.0f})};
    test_support::requireVec3(mesh.getCenter(), {1.0f, 1.0f, 2.0f});
}

TEST_CASE("Mesh global normal normalizes the summed vertex normals", "[mesh]")
{
    // A 3-4-5 vector gives independent exact expectations for normalization.
    Mesh mesh;
    mesh.m_vertices = {sampleVertex({0, 0, 0}, {3, 0, 0}),
                       sampleVertex({1, 0, 0}, {0, 4, 0})};
    test_support::requireVec3(mesh.getGlobalNormal(), {0.6f, 0.8f, 0.0f});
}

TEST_CASE("Mesh picking returns the closest parameter regardless of triangle order", "[mesh][geometry]")
{
    // Two parallel triangles hit at t=1 and t=0.5 for a nonunit ray: first-hit is wrong.
    Mesh mesh;
    mesh.m_vertices = {sampleVertex({0, 0, 0}), sampleVertex({1, 0, 0}), sampleVertex({0, 1, 0}),
                       sampleVertex({0, 0, 1}), sampleVertex({1, 0, 1}), sampleVertex({0, 1, 1})};
    mesh.m_triangleData.emplace_back(0, 1, 2, mesh.m_vertices);
    mesh.m_triangleData.emplace_back(3, 4, 5, mesh.m_vertices);
    const Ray ray({0.25f, 0.25f, 2.0f}, {0.0f, 0.0f, -2.0f});
    float parameter = -10.0f;
    REQUIRE(mesh.intersectRayTriangle(ray, parameter));
    REQUIRE(parameter == Catch::Approx(0.5f).margin(1e-6f));
    // Rebuild only the triangle cache: borrowed vertex references remain valid.
    mesh.m_triangleData.clear();
    mesh.m_triangleData.emplace_back(3, 4, 5, mesh.m_vertices);
    mesh.m_triangleData.emplace_back(0, 1, 2, mesh.m_vertices);
    parameter = -10.0f;
    REQUIRE(mesh.intersectRayTriangle(ray, parameter));
    REQUIRE(parameter == Catch::Approx(0.5f).margin(1e-6f));
}

TEST_CASE("Mesh miss preserves the caller's output parameter", "[mesh][negative]")
{
    // An unsuccessful pick must not replace the previous selection distance with garbage.
    Mesh mesh;
    mesh.m_vertices = {sampleVertex({0, 0, 0}), sampleVertex({1, 0, 0}), sampleVertex({0, 1, 0})};
    mesh.m_triangleData.emplace_back(0, 1, 2, mesh.m_vertices);
    float parameter = 42.0f;
    REQUIRE_FALSE(mesh.intersectRayTriangle(Ray({2, 2, 1}, {0, 0, -1}), parameter));
    REQUIRE(parameter == 42.0f);
}

TEST_CASE("Empty mesh picking rejects without touching output", "[mesh][negative]")
{
    // Empty caches are possible after loading failure, even though centroid is undefined there.
    const Mesh mesh;
    float parameter = 7.0f;
    REQUIRE_FALSE(mesh.intersectRayTriangle(Ray({0, 0, 1}, {0, 0, -1}), parameter));
    REQUIRE(parameter == 7.0f);
}