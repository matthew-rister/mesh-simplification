#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_CAMERA_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_CAMERA_H_

#include <glm/gtc/matrix_transform.hpp>
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
  Camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const ViewFrustum& view_frustum)
      : view_transform_{glm::lookAt(eye, center, up)},
        projection_transform_{glm::perspective(view_frustum.field_of_view_y,
                                               view_frustum.aspect_ratio,
                                               view_frustum.z_near,
                                               view_frustum.z_far)} {
    projection_transform_[1][1] *= -1;  // account for inverted y-axis convention in OpenGL
  }

  [[nodiscard]] const glm::mat4& view_transform() const noexcept { return view_transform_; }
  [[nodiscard]] const glm::mat4& projection_transform() const noexcept { return projection_transform_; }

private:
  glm::mat4 view_transform_, projection_transform_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_CAMERA_H_
