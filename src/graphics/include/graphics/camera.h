#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_CAMERA_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_CAMERA_H_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace gfx {

struct ViewFrustum {
  float field_of_view_y;
  float aspect_ratio;
  float z_near;
  float z_far;
};

class Camera {
public:
  Camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const ViewFrustum& view_frustum);

  void Translate(const float dx, const float dy, const float dz);
  void Rotate(const float theta, const float phi);

  [[nodiscard]] const glm::mat4& view_transform() const noexcept { return view_transform_; }
  [[nodiscard]] const glm::mat4& projection_transform() const noexcept { return projection_transform_; }

private:
  glm::mat4 view_transform_, projection_transform_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_CAMERA_H_
