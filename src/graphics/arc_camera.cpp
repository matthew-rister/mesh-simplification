#include "graphics/arc_camera.h"

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {

glm::mat4 GetViewTransform(const glm::vec3& target, const gfx::SphericalCoordinates& spherical_position) {
  static constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};
  const auto cartesian_position = target + gfx::ToCartesianCoordinates(spherical_position);
  return glm::lookAt(cartesian_position, target, kUp);
}

glm::mat4 GetProjectionTransform(const gfx::ArcCamera::ViewFrustum& view_frustum) {
  const auto [field_of_view_y, aspect_ratio, z_near, z_far] = view_frustum;
  auto projection_transform = glm::perspective(field_of_view_y, aspect_ratio, z_near, z_far);
  projection_transform[1][1] *= -1;  // account for inverted y-axis convention in OpenGL
  return projection_transform;
}

}  // namespace

gfx::ArcCamera::ArcCamera(const glm::vec3& target, const glm::vec3& position, const ViewFrustum& view_frustum)
    : target_{target},
      position_{ToSphericalCoordinates(position - target)},
      view_transform_{GetViewTransform(target_, position_)},
      projection_transform_{GetProjectionTransform(view_frustum)} {}

void gfx::ArcCamera::Rotate(const float theta, const float phi) {
  static constexpr auto kThetaMax = glm::two_pi<float>();
  static constexpr auto kPhiMax = glm::radians(89.0f);
  position_.theta = std::fmodf(position_.theta + theta, kThetaMax);
  position_.phi = std::clamp(position_.phi + phi, -kPhiMax, kPhiMax);
  view_transform_ = GetViewTransform(target_, position_);
}
