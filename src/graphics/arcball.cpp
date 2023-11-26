#include "graphics/arcball.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include <glm/glm.hpp>

namespace {

glm::vec2 GetNormalizedDeviceCoordinates(const glm::vec2& cursor_position, const gfx::Window::Size& window_size) {
  static constexpr auto kMinCoordinateValue = -1.0f;
  static constexpr auto kMaxCoordinateValue = 1.0f;

  const auto width = static_cast<float>(window_size.width);
  const auto height = static_cast<float>(window_size.height);
  assert(width > 0.0f);
  assert(height > 0.0f);

  const auto x = std::clamp(cursor_position.x * 2.0f / width - 1.0f, kMinCoordinateValue, kMaxCoordinateValue);
  const auto y = std::clamp(cursor_position.y * 2.0f / height - 1.0f, kMinCoordinateValue, kMaxCoordinateValue);

  return glm::vec2{x, -y};
}

glm::vec3 GetArcballPosition(const glm::vec2& cursor_position, const gfx::Window::Size& window_size) {
  const auto cursor_position_ndc = GetNormalizedDeviceCoordinates(cursor_position, window_size);
  const auto x = cursor_position_ndc.x;
  const auto y = cursor_position_ndc.y;

  if (const auto n = x * x + y * y; n <= 1.0f) {
    return glm::vec3{x, y, std::sqrt(1.0f - n)};
  }

  return glm::normalize(glm::vec3{x, y, 0.0f});
}

}  // namespace

gfx::arcball::Rotation gfx::arcball::GetRotation(const glm::vec2& cursor_position_start,
                                                 const glm::vec2& cursor_position_end,
                                                 const Window::Size& window_size) {
  const auto arcball_position_start = GetArcballPosition(cursor_position_start, window_size);
  const auto arcball_position_end = GetArcballPosition(cursor_position_end, window_size);

  return Rotation{.axis = glm::cross(arcball_position_start, arcball_position_end),
                  .angle = std::acos(std::min(1.0f, glm::dot(arcball_position_start, arcball_position_end)))};
}
