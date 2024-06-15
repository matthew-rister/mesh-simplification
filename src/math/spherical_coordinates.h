#ifndef MATH_SPHERICAL_COORDINATES_H_
#define MATH_SPHERICAL_COORDINATES_H_

#include <glm/fwd.hpp>

namespace gfx {

struct SphericalCoordinates {
  float radius = 0.0f;
  float theta = 0.0f;
  float phi = 0.0f;
};

[[nodiscard]] SphericalCoordinates ToSphericalCoordinates(const glm::vec3& cartesian_coordinates);
[[nodiscard]] glm::vec3 ToCartesianCoordinates(const SphericalCoordinates& spherical_coordinates);

}  // namespace gfx

#endif  // MATH_SPHERICAL_COORDINATES_H_
