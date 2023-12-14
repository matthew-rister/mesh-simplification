#ifndef SRC_GRAPHICS_ARCBALL_H_
#define SRC_GRAPHICS_ARCBALL_H_

#include <optional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "graphics/window.h"

namespace gfx::arcball {

/** \brief A rotation about an arbitrary axis. */
struct Rotation {
  glm::vec3 axis;
  float angle;
};

/**
 * \brief Gets the axis and angle to rotate a mesh using changes in cursor position.
 * \param cursor_position_start The starting cursor position.
 * \param cursor_position_end The ending cursor position.
 * \param window_extent The window width and height.
 * \return The axis (in view space) and angle (in radians) to rotate the mesh if the angle between the arcball positions
 *         of \p cursor_position_start and \p cursor_position_end is nonzero, otherwise \c std::nullopt.
 * \see https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
 */
std::optional<Rotation> GetRotation(const glm::vec2& cursor_position_start,
                                    const glm::vec2& cursor_position_end,
                                    const Window::Extent& window_extent);

}  // namespace gfx::arcball

#endif  // SRC_GRAPHICS_ARCBALL_H_
