#include "geometry/face.h"

#include <algorithm>
#include <array>

#include <glm/geometric.hpp>

namespace {

std::array<std::shared_ptr<gfx::Vertex>, 3> GetMinVertexOrder(const std::shared_ptr<gfx::Vertex>& v0,
                                                              const std::shared_ptr<gfx::Vertex>& v1,
                                                              const std::shared_ptr<gfx::Vertex>& v2) {
  const auto min_id = std::min({v0->id(), v1->id(), v2->id()});
  if (min_id == v0->id()) return std::array{v0, v1, v2};
  if (min_id == v1->id()) return std::array{v1, v2, v0};
  return std::array{v2, v0, v1};
}

}  // namespace

namespace gfx {

Face::Face(const std::shared_ptr<Vertex>& v0,  // NOLINT(*-member-init)
           const std::shared_ptr<Vertex>& v1,
           const std::shared_ptr<Vertex>& v2) noexcept {
  tie(v0_, v1_, v2_) = GetMinVertexOrder(v0, v1, v2);

  const auto edge01 = v1_.lock()->position() - v0_.lock()->position();
  const auto edge02 = v2_.lock()->position() - v0_.lock()->position();
  const auto normal = glm::cross(edge01, edge02);

  const auto normal_magnitude = glm::length(normal);
  assert(normal_magnitude > 0.0f);  // ensure face vertices are not collinear
  area_ = 0.5f * normal_magnitude;  // NOLINT(*-magic-numbers)
  normal_ = normal / normal_magnitude;
}

}  // namespace gfx
