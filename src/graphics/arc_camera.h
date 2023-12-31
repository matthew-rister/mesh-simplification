#ifndef SRC_GRAPHICS_ARC_CAMERA_H_
#define SRC_GRAPHICS_ARC_CAMERA_H_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "math/spherical_coordinates.h"

namespace gfx {

class ArcCamera {
public:
  struct ViewFrustum {
    float field_of_view_y{};
    float aspect_ratio{};
    float z_near{};
    float z_far{};
  };

  ArcCamera(const glm::vec3& target, const glm::vec3& position, const ViewFrustum& view_frustum);

  [[nodiscard]] const glm::mat4& view_transform() const noexcept { return view_transform_; }
  [[nodiscard]] const glm::mat4& projection_transform() const noexcept { return projection_transform_; }

  void Translate(float dx, float dy, float dz);
  void Rotate(float theta, float phi);

private:
  glm::vec3 target_;
  SphericalCoordinates position_;
  glm::mat4 view_transform_;
  glm::mat4 projection_transform_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_ARC_CAMERA_H_
