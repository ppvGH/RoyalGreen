#pragma once

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/glm.hpp>

namespace test_support
{
/**
 * @brief Compare floating-point vector components with an absolute tolerance.
 * @details An absolute margin also works for zero and negative coordinates.
 * Disabling Catch's relative tolerance keeps large coordinates from hiding
 * errors. These helpers compare results; they do not reproduce production math.
 */
template<glm::length_t Length, glm::qualifier Qualifier>
inline void requireVector(const glm::vec<Length, float, Qualifier>& actual,
                          const glm::vec<Length, float, Qualifier>& expected,
                          float margin = 1e-5f)
{
    for (glm::length_t component = 0; component < Length; ++component)
    {
        INFO("vector component " << component);
        REQUIRE(actual[component] ==
                Catch::Approx(expected[component]).epsilon(0).margin(margin));
    }
}

inline void requireVec2(const glm::vec2& actual, const glm::vec2& expected,
                        float margin = 1e-5f)
{
    requireVector(actual, expected, margin);
}

inline void requireVec3(const glm::vec3& actual, const glm::vec3& expected,
                        float margin = 1e-5f)
{
    requireVector(actual, expected, margin);
}

inline void requireVec4(const glm::vec4& actual, const glm::vec4& expected,
                        float margin = 1e-5f)
{
    requireVector(actual, expected, margin);
}

/** @brief Compare GLM's column-major matrix components without a second algorithm. */
inline void requireMat4(const glm::mat4& actual, const glm::mat4& expected,
                        float margin = 1e-5f)
{
    for (glm::length_t column = 0; column < 4; ++column)
    {
        INFO("matrix column " << column);
        requireVec4(actual[column], expected[column], margin);
    }
}
} // namespace test_support
