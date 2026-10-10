#pragma once
#include <cmath>
#include <stdexcept>
#include <glm/glm.hpp>
namespace validation {
inline void require(bool condition, const char* message) {
    if (!condition) throw std::invalid_argument(message);
}
inline bool finite(float value) { return std::isfinite(value); }
template<glm::length_t L, typename T, glm::qualifier Q>
bool finite(const glm::vec<L,T,Q>& value) {
    for (glm::length_t i=0; i<L; ++i) if (!std::isfinite(value[i])) return false;
    return true;
}
inline void time(float dt) { require(finite(dt) && dt >= 0, "Elapsed time must be finite and nonnegative"); }
}
