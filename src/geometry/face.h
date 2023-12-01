#ifndef SRC_GEOMETRY_FACE_H_
#define SRC_GEOMETRY_FACE_H_

#include <cassert>
#include <memory>

#include <glm/vec3.hpp>

#include "geometry/vertex.h"

namespace gfx {

class Face {
public:
  Face(const std::shared_ptr<Vertex>& v0,
       const std::shared_ptr<Vertex>& v1,
       const std::shared_ptr<Vertex>& v2) noexcept;

  [[nodiscard]] std::shared_ptr<Vertex> v0() const noexcept {
    assert(!v0_.expired());
    return v0_.lock();
  }

  [[nodiscard]] std::shared_ptr<Vertex> v1() const noexcept {
    assert(!v1_.expired());
    return v1_.lock();
  }

  [[nodiscard]] std::shared_ptr<Vertex> v2() const noexcept {
    assert(!v2_.expired());
    return v2_.lock();
  }

  [[nodiscard]] const glm::vec3& normal() const noexcept { return normal_; }

  [[nodiscard]] float area() const noexcept { return area_; }

private:
  std::weak_ptr<Vertex> v0_, v1_, v2_;
  glm::vec3 normal_;
  float area_;
};

inline std::size_t hash_value(const Face& face) noexcept {
  std::size_t seed = 0;
  hash_combine(seed, *face.v0());
  hash_combine(seed, *face.v1());
  hash_combine(seed, *face.v2());
  return seed;
}

}  // namespace gfx

#endif  // SRC_GEOMETRY_FACE_H_
