#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_ARCBALL_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_ARCBALL_H_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "graphics/window.h"

namespace gfx::arcball {

struct Rotation {
  glm::vec3 axis;
  float angle;
};

Rotation GetRotation(const glm::vec2& cursor_position_start,
                     const glm::vec2& cursor_position_end,
                     const Window::Size& window_size);

}  // namespace gfx::arcball

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_ARCBALL_H_
