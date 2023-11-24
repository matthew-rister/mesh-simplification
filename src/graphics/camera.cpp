#include "graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>

gfx::Camera::Camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const ViewFrustum& view_frustum)
    : view_transform_{glm::lookAt(eye, center, up)},
      projection_transform_{glm::perspective(view_frustum.field_of_view_y,
                                             view_frustum.aspect_ratio,
                                             view_frustum.z_near,
                                             view_frustum.z_far)} {
  projection_transform_[1][1] *= -1;  // account for inverted y-axis convention in OpenGL
}
void gfx::Camera::Translate(const float /*dx*/, const float /*dy*/, const float /*dz*/) {}

void gfx::Camera::Rotate(const float /*theta*/, const float /*phi*/) {}
