#pragma once

#include <cassert>
#include <cmath>
#include <format>
#include <ostream>

namespace gfx {

struct Vector3 {
  constexpr Vector3() noexcept = default;
  constexpr Vector3(const float x, const float y, const float z) noexcept : x{x}, y{y}, z{z} {}

  float x{}, y{}, z{};
};

constexpr Vector3 operator+(const Vector3& u, const Vector3& v) noexcept {
  return Vector3{u.x + v.x, u.y + v.y, u.z + v.z};
}

constexpr Vector3 operator-(const Vector3& u, const Vector3& v) noexcept {
  return Vector3{u.x - v.x, u.y - v.y, u.z - v.z};
}

constexpr Vector3 operator*(const float s, const Vector3& v) noexcept { return Vector3{s * v.x, s * v.y, s * v.z}; }

constexpr Vector3 operator/(const Vector3& v, const float s) noexcept {
  assert(s != 0.0f);
  return 1.0f / s * v;
}

constexpr bool operator==(const Vector3& u, const Vector3& v) noexcept {
  return u.x == v.x && u.y == v.y && u.z == v.z;
}

constexpr bool operator!=(const Vector3& u, const Vector3& v) noexcept { return !(u == v); }

inline std::ostream& operator<<(std::ostream& os, const Vector3& v) {
  return os << std::format("({},{},{})", v.x, v.y, v.z);
}

inline float Length(const Vector3& v) noexcept { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

inline Vector3 Normalize(const Vector3& v) noexcept {
  const auto length = Length(v);
  assert(length > 0.0f);
  return v / length;
}

}  // namespace gfx
