#include "graphics/arc_camera.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gfx {

ArcCamera::ArcCamera(const glm::vec3& target, const glm::vec3& position, const ViewFrustum& view_frustum)
    : target_{target}, position_{ToSphericalCoordinates(position - target)}, view_frustum_{view_frustum} {}

glm::mat4 ArcCamera::GetViewTransform() const noexcept {
  static constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};
  const auto cartesian_position = target_ + ToCartesianCoordinates(position_);
  return glm::lookAt(cartesian_position, target_, kUp);
}

glm::mat4 ArcCamera::GetProjectionTransform() const noexcept {
  const auto [field_of_view_y, aspect_ratio, z_near, z_far] = view_frustum_;
  auto projection_transform = glm::perspective(field_of_view_y, aspect_ratio, z_near, z_far);
  projection_transform[1][1] *= -1;  // account for inverted y-axis convention in OpenGL
  return projection_transform;
}

void ArcCamera::Translate(const float dx, const float dy, const float dz) {
  const glm::mat3 view_transform = GetViewTransform();
  target_ += glm::vec3{dx, dy, dz} * view_transform;
}

void ArcCamera::Rotate(const float theta, const float phi) {
  static constexpr auto kThetaMax = glm::two_pi<float>();
  static constexpr auto kPhiMax = glm::radians(89.0f);
  position_.theta = std::fmod(position_.theta + theta, kThetaMax);
  position_.phi = std::clamp(position_.phi + phi, -kPhiMax, kPhiMax);
}

void ArcCamera::Zoom(const float rate) {
  static constexpr auto kEpsilon = std::numeric_limits<float>::epsilon();
  position_.radius = std::max((1.0f - rate) * position_.radius, kEpsilon);
}

}  // namespace gfx
