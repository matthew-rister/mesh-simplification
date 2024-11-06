#ifndef GRAPHICS_ARC_CAMERA_H_
#define GRAPHICS_ARC_CAMERA_H_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "math/spherical_coordinates.h"

namespace gfx {

struct ViewFrustum {
  float field_of_view_y = 0.0f;
  float aspect_ratio = 0.0f;
  float z_near = 0.0f;
  float z_far = 0.0f;
};

class ArcCamera {
public:
  ArcCamera(const glm::vec3& target, const glm::vec3& position, const ViewFrustum& view_frustum);

  [[nodiscard]] glm::mat4 GetViewTransform() const noexcept;
  [[nodiscard]] glm::mat4 GetProjectionTransform() const noexcept;

  void Translate(float dx, float dy, float dz);
  void Rotate(float theta, float phi);
  void Zoom(float rate);

private:
  glm::vec3 target_{0.0f};
  SphericalCoordinates position_;
  ViewFrustum view_frustum_;
};

}  // namespace gfx

#endif  // GRAPHICS_ARC_CAMERA_H_
