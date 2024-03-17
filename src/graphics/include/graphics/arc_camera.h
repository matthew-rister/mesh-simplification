#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_ARC_CAMERA_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_ARC_CAMERA_H_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "math/spherical_coordinates.h"

namespace gfx {

/** \brief A view frustum used in perspective projection. */
struct ViewFrustum {
  /** \brief The field of view vertical angle in degrees. */
  float field_of_view_y{};

  /** \brief The ratio of the view frustum's width by height. */
  float aspect_ratio{};

  /** \brief The distance to the near plane. */
  float z_near{};

  /** \brief The distance to the far plane. */
  float z_far{};
};

/** \brief A camera that rotates around a fixed point in space. */
class ArcCamera {
public:
  /**
   * \brief Initializes an arc camera.
   * \param target The target position to rotate around.
   * \param position The initial camera position.
   * \param view_frustum The view frustum used in perspective projection.
   */
  ArcCamera(const glm::vec3& target, const glm::vec3& position, const ViewFrustum& view_frustum);

  /** \brief Gets the view transformation matrix. */
  [[nodiscard]] glm::mat4 GetViewTransform() const noexcept;

  /** \brief Gets the perspective projection transformation matrix. */
  [[nodiscard]] glm::mat4 GetProjectionTransform() const noexcept;

  /**
   * \brief Translates the camera along its coordinate axes.
   * \param translation The amount to translate the camera in the x,y,z directions.
   */
  void Translate(const glm::vec3& translation);

  /**
   * \brief Rotates the camera along its spherical angles.
   * \param theta The azimuthal angle to rotate the camera by.
   * \param phi The polar angle to rotate the camera by.
   */
  void Rotate(float theta, float phi);

  /**
   * \brief Moves the camera closer or farther away from its target position.
   * \param rate The zoom rate (e.g., .15 indicates zoom in by 15%).
   */
  void Zoom(float rate);

private:
  glm::vec3 target_;
  SphericalCoordinates position_;
  ViewFrustum view_frustum_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_ARC_CAMERA_H_
