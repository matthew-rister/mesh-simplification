#ifndef SRC_MATH_SPHERICAL_COORDINATES_H_
#define SRC_MATH_SPHERICAL_COORDINATES_H_

#include <cassert>
#include <cmath>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace gfx {

struct SphericalCoordinates {
  float radius{};
  float theta{};
  float phi{};
};

[[nodiscard]] inline SphericalCoordinates ToSphericalCoordinates(const glm::vec3& cartesian_coordinates) {
  const auto radius = glm::length(cartesian_coordinates);
  assert(radius > 0.0f);
  return SphericalCoordinates{.radius = radius,
                              .theta = std::atan2f(cartesian_coordinates.x, cartesian_coordinates.z),
                              .phi = std::asinf(-cartesian_coordinates.y / radius)};
}

[[nodiscard]] inline glm::vec3 ToCartesianCoordinates(const SphericalCoordinates& spherical_coordinates) {
  const auto [radius, theta, phi] = spherical_coordinates;
  const auto cos_phi = std::cosf(phi);
  const auto x = radius * std::sinf(theta) * cos_phi;
  const auto y = radius * std::sinf(-phi);
  const auto z = radius * std::cosf(theta) * cos_phi;
  return glm::vec3{x, y, z};
}

}  // namespace gfx

#endif  // SRC_MATH_SPHERICAL_COORDINATES_H_
