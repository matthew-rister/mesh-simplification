#include "math/spherical_coordinates.h"

#include <array>
#include <cstdint>

#include <gtest/gtest.h>
#include <glm/gtc/constants.hpp>

namespace {

constexpr auto kEpsilon = 1.0e-6f;
constexpr auto kHalfPi = glm::half_pi<float>();
constexpr auto kPi = glm::pi<float>();
constexpr auto kRadius = 2.0f;

TEST(SphericalCoordinatesTest, ConvertCartesianCoordiantesAtTheOriginToSphericalCoordinates) {
  static constexpr glm::vec3 kCartesianPosition{0.0f};
  const auto spherical_position = gfx::ToSphericalCoordinates(kCartesianPosition);
  EXPECT_FLOAT_EQ(spherical_position.radius, 0.0f);
  EXPECT_FLOAT_EQ(spherical_position.theta, 0.0f);
  EXPECT_FLOAT_EQ(spherical_position.phi, 0.0f);
}

TEST(SphericalCoordinatesTest, ConvertCartesianCoordinatesOnThePositiveZAxisToSphericalCoordinates) {
  static constexpr glm::vec3 kCartesianPosition{0.0f, 0.0f, kRadius};
  const auto spherical_position = gfx::ToSphericalCoordinates(kCartesianPosition);
  EXPECT_FLOAT_EQ(spherical_position.radius, kRadius);
  EXPECT_FLOAT_EQ(spherical_position.theta, 0.0f);
  EXPECT_FLOAT_EQ(spherical_position.phi, 0.0f);
}

TEST(SphericalCoordinatesTest, ConvertCartesianCoordinatesOnTheNegativeZAxisToSphericalCoordinates) {
  static constexpr glm::vec3 kCartesianPosition{0.0f, 0.0f, -kRadius};
  const auto spherical_position = gfx::ToSphericalCoordinates(kCartesianPosition);
  EXPECT_FLOAT_EQ(spherical_position.radius, kRadius);
  EXPECT_FLOAT_EQ(spherical_position.theta, kPi);
  EXPECT_FLOAT_EQ(spherical_position.phi, 0.0f);
}

TEST(SphericalCoordinatesTest, ConvertCartesianCoordinatesOnThePositiveXAxisToSphericalCoordinates) {
  static constexpr glm::vec3 kCartesianPosition{kRadius, 0.0f, 0.0f};
  const auto spherical_position = gfx::ToSphericalCoordinates(kCartesianPosition);
  EXPECT_FLOAT_EQ(spherical_position.radius, kRadius);
  EXPECT_FLOAT_EQ(spherical_position.theta, kHalfPi);
  EXPECT_FLOAT_EQ(spherical_position.phi, 0.0f);
}

TEST(SphericalCoordinatesTest, ConvertCartesianCoordinatesOnTheNegativeXAxisToSphericalCoordinates) {
  static constexpr glm::vec3 kPosition{-kRadius, 0.0f, 0.0f};
  const auto spherical_position = gfx::ToSphericalCoordinates(kPosition);
  EXPECT_FLOAT_EQ(spherical_position.radius, kRadius);
  EXPECT_FLOAT_EQ(spherical_position.theta, -kHalfPi);
  EXPECT_FLOAT_EQ(spherical_position.phi, 0.0f);
}

TEST(SphericalCoordinatesTest, ConvertCartesianCoordinatesOnThePositiveYAxisToSphericalCoordinates) {
  static constexpr glm::vec3 kCartesianPosition{0.0f, kRadius, 0.0f};
  const auto spherical_position = gfx::ToSphericalCoordinates(kCartesianPosition);
  EXPECT_FLOAT_EQ(spherical_position.radius, kRadius);
  EXPECT_FLOAT_EQ(spherical_position.theta, 0.0f);
  EXPECT_FLOAT_EQ(spherical_position.phi, -kHalfPi);
}

TEST(SphericalCoordinatesTest, ConvertCartesianCoordinatesOnTheNegativeYAxisToSphericalCoordinates) {
  static constexpr glm::vec3 kPosition{0.0f, -kRadius, 0.0f};
  const auto spherical_position = gfx::ToSphericalCoordinates(kPosition);
  EXPECT_FLOAT_EQ(spherical_position.radius, kRadius);
  EXPECT_FLOAT_EQ(spherical_position.theta, 0.0f);
  EXPECT_FLOAT_EQ(spherical_position.phi, kHalfPi);
}

TEST(SphericalCoordinatesTest, ConvertSphericalCoordinatesOnThePositiveZAxisToCartesianCoordinates) {
  static constexpr gfx::SphericalCoordinates kSphericalCoordinates{.radius = kRadius, .theta = 0.0f, .phi = 0.0f};
  const auto cartesian_position = gfx::ToCartesianCoordinates(kSphericalCoordinates);
  EXPECT_NEAR(cartesian_position.x, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.y, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.z, kRadius, kEpsilon);
}

TEST(SphericalCoordinatesTest, ConvertSphericalCoordinatesOnTheNegativeZAxisToCartesianCoordinates) {
  static constexpr gfx::SphericalCoordinates kSphericalCoordinates{.radius = kRadius, .theta = kPi, .phi = 0.0f};
  const auto cartesian_position = gfx::ToCartesianCoordinates(kSphericalCoordinates);
  EXPECT_NEAR(cartesian_position.x, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.y, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.z, -kRadius, kEpsilon);
}

TEST(SphericalCoordinatesTest, ConvertSphericalCoordinatesOnThePositiveXAxisToCartesianCoordinates) {
  static constexpr gfx::SphericalCoordinates kSphericalCoordinates{.radius = kRadius, .theta = kHalfPi, .phi = 0.0f};
  const auto cartesian_position = gfx::ToCartesianCoordinates(kSphericalCoordinates);
  EXPECT_NEAR(cartesian_position.x, kRadius, kEpsilon);
  EXPECT_NEAR(cartesian_position.y, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.z, 0.0f, kEpsilon);
}

TEST(SphericalCoordinatesTest, ConvertSphericalCoordinatesOnTheNegativeXAxisToCartesianCoordinates) {
  static constexpr gfx::SphericalCoordinates kSphericalCoordinates{.radius = kRadius, .theta = -kHalfPi, .phi = 0.0f};
  const auto cartesian_position = gfx::ToCartesianCoordinates(kSphericalCoordinates);
  EXPECT_NEAR(cartesian_position.x, -kRadius, kEpsilon);
  EXPECT_NEAR(cartesian_position.y, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.z, 0.0f, kEpsilon);
}

TEST(SphericalCoordinatesTest, ConvertSphericalCoordinatesOnThePositiveYAxisToCartesianCoordinates) {
  static constexpr gfx::SphericalCoordinates kSphericalCoordinates{.radius = kRadius, .theta = 0.0f, .phi = -kHalfPi};
  const auto cartesian_position = gfx::ToCartesianCoordinates(kSphericalCoordinates);
  EXPECT_NEAR(cartesian_position.x, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.y, kRadius, kEpsilon);
  EXPECT_NEAR(cartesian_position.z, 0.0f, kEpsilon);
}

TEST(SphericalCoordinatesTest, ConvertSphericalCoordinatesOnTheNegativeYAxisToCartesianCoordinates) {
  static constexpr gfx::SphericalCoordinates kSphericalCoordinates{.radius = kRadius, .theta = 0.0f, .phi = kHalfPi};
  const auto cartesian_position = gfx::ToCartesianCoordinates(kSphericalCoordinates);
  EXPECT_NEAR(cartesian_position.x, 0.0f, kEpsilon);
  EXPECT_NEAR(cartesian_position.y, -kRadius, kEpsilon);
  EXPECT_NEAR(cartesian_position.z, 0.0f, kEpsilon);
}

}  // namespace
