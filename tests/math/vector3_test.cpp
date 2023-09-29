#include "vector3.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE("Vector3 construction") {
  SECTION("A default constructed vector initializes each component to zero") {
    constexpr gfx::Vector3 v;
    STATIC_REQUIRE(v.x == 0.0f);
    STATIC_REQUIRE(v.y == 0.0f);
    STATIC_REQUIRE(v.z == 0.0f);
  }

  SECTION("A vector constructed with (x,y,z) values has the correct component values") {
    constexpr auto x = 1.0f, y = 2.0f, z = 3.0f;
    constexpr gfx::Vector3 v{x, y, z};
    STATIC_REQUIRE(v.x == x);
    STATIC_REQUIRE(v.y == y);
    STATIC_REQUIRE(v.z == z);
  }
}

TEST_CASE("Vector3 operations") {
  constexpr gfx::Vector3 u{1.0f, 2.0f, 3.0f};
  constexpr gfx::Vector3 v{2.0f, 4.0f, 8.0f};
  constexpr auto s = 2.0f;

  SECTION("Adding two vectors adds respective components in each vector") {
    constexpr auto w = u + v;
    STATIC_REQUIRE(w.x == u.x + v.x);
    STATIC_REQUIRE(w.y == u.y + v.y);
    STATIC_REQUIRE(w.z == u.z + v.z);
  }

  SECTION("Subtracting two vectors subtracts respective components in each vector") {
    constexpr auto w = u - v;
    STATIC_REQUIRE(w.x == u.x - v.x);
    STATIC_REQUIRE(w.y == u.y - v.y);
    STATIC_REQUIRE(w.z == u.z - v.z);
  }

  SECTION("Multiplying a vector by a scalar multiples each component by that scalar") {
    constexpr auto w = s * u;
    STATIC_REQUIRE(w.x == s * u.x);
    STATIC_REQUIRE(w.y == s * u.y);
    STATIC_REQUIRE(w.z == s * u.z);
  }

  SECTION("Dividing a vector by a scalar divides each component by that scalar") {
    constexpr auto w = u / s;
    STATIC_REQUIRE(w.x == u.x / s);
    STATIC_REQUIRE(w.y == u.y / s);
    STATIC_REQUIRE(w.z == u.z / s);
  }

  SECTION("Two vectors are equal if all of their components are equal") {
    STATIC_REQUIRE(v == gfx::Vector3{v.x, v.y, v.z});
  }

  SECTION("Two vectors are not equal if any of their components are not equal") {
    STATIC_REQUIRE(v != gfx::Vector3{v.x + 1.0f, v.y, v.z});
    STATIC_REQUIRE(v != gfx::Vector3{v.x, v.y + 1.0f, v.z});
    STATIC_REQUIRE(v != gfx::Vector3{v.x, v.y, v.z + 1.0f});
  }

  SECTION("Inserting a vector to an output stream writes its component valeus") {
    std::ostringstream oss;
    oss << gfx::Vector3{1.0f, 2.0f, 3.0f};
    REQUIRE(oss.str() == "(1,2,3)");
  }
}

TEST_CASE("Vector3 length") {
  SECTION("The length of the zero vector is zero") { REQUIRE(gfx::Length(gfx::Vector3{0.0f, 0.0f, 0.0f}) == 0.0f); }

  SECTION("The length of a vector is the square root of the sum of each component squared") {
    constexpr gfx::Vector3 v{1.0f, 2.0f, 3.0f};
    REQUIRE(gfx::Length(v) == std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
  }
}

TEST_CASE("Vector3 normalization") {
  constexpr gfx::Vector3 v{1.0f, 2.0f, 3.0f};
  const auto v_norm = gfx::Normalize(v);

  SECTION("Normalizing a vector divides each component by the length of the vector") {
    const auto v_length = gfx::Length(v);
    REQUIRE(v_norm.x == v.x / v_length);
    REQUIRE(v_norm.y == v.y / v_length);
    REQUIRE(v_norm.z == v.z / v_length);
  }

  SECTION("The length of a normalized vector is one") {
    REQUIRE_THAT(gfx::Length(v_norm), Catch::Matchers::WithinRel(1.0f));
  }
}
