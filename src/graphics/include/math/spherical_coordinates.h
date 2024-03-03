#ifndef SRC_GRAPHICS_INCLUDE_MATH_SPHERICAL_COORDINATES_H_
#define SRC_GRAPHICS_INCLUDE_MATH_SPHERICAL_COORDINATES_H_

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
  return radius == 0.0f ? SphericalCoordinates{.radius = 0.0f, .theta = 0.0f, .phi = 0.0f}
                        : SphericalCoordinates{.radius = radius,
                                               .theta = std::atan2f(cartesian_coordinates.x, cartesian_coordinates.z),
                                               .phi = std::asinf(-cartesian_coordinates.y / radius)};
}

[[nodiscard]] inline glm::vec3 ToCartesianCoordinates(const SphericalCoordinates& spherical_coordinates) {
  const auto [radius, theta, phi] = spherical_coordinates;
  const auto cos_phi = std::cos(phi);
  const auto x = radius * std::sin(theta) * cos_phi;
  const auto y = radius * std::sin(-phi);
  const auto z = radius * std::cos(theta) * cos_phi;
  return glm::vec3{x, y, z};
}

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_MATH_SPHERICAL_COORDINATES_H_
