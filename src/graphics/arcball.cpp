#include "graphics/arcball.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace {

constexpr glm::vec2 GetNormalizedViewPosition(const glm::vec2& cursor_position, const gfx::Window::Size& window_size) {
  constexpr auto kMinCoordinateValue = -1.0f;
  constexpr auto kMaxCoordinateValue = 1.0f;

  const auto width = static_cast<float>(window_size.width);
  const auto height = static_cast<float>(window_size.height);
  assert(width > 0.0f);
  assert(height > 0.0f);

  // normalize cursor position to [-1, 1] using clamp to handle cursor positions outside the window bounds
  const auto x = std::clamp(cursor_position.x * 2.0f / width - 1.0f, kMinCoordinateValue, kMaxCoordinateValue);
  const auto y = std::clamp(cursor_position.y * 2.0f / height - 1.0f, kMinCoordinateValue, kMaxCoordinateValue);

  return glm::vec2{x, -y};  // y-coordinate negated to account for +y-axis up convention
}

glm::vec3 GetArcballPosition(const glm::vec2& cursor_position, const gfx::Window::Size& window_size) {
  const auto view_position = GetNormalizedViewPosition(cursor_position, window_size);
  const auto a = view_position.x;
  const auto b = view_position.y;

  // compute z using the standard equation for a unit sphere (x^2 + y^2 + z^2 = 1)
  if (const auto c = a * a + b * b; c < 1.0f) {
    return glm::vec3{a, b, std::sqrt(1.0f - c)};
  }

  // get the nearest position on the arcball
  return glm::normalize(glm::vec3{a, b, 0.0f});
}

}  // namespace

std::optional<gfx::arcball::Rotation> gfx::arcball::GetRotation(const glm::vec2& cursor_position_start,
                                                                const glm::vec2& cursor_position_end,
                                                                const Window::Size& window_size) {
  const auto arcball_position_start = GetArcballPosition(cursor_position_start, window_size);
  const auto arcball_position_end = GetArcballPosition(cursor_position_end, window_size);

  // avoid calculating the cross product and angle between identical vectors
  if (arcball_position_start == arcball_position_end) return std::nullopt;

  return Rotation{
      .axis = glm::cross(arcball_position_start, arcball_position_end),
      // use min to account for numerical issues where the dot product is greater than 1 causing acos to produce NaN
      .angle = std::acos(std::min(1.0f, glm::dot(arcball_position_start, arcball_position_end)))};
}
