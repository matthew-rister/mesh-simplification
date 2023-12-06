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

inline std::size_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) noexcept {
  std::size_t seed = 0;
  hash_combine(seed, v0);
  hash_combine(seed, v1);
  hash_combine(seed, v2);
  return seed;
}

inline std::size_t hash_value(const Face& face) noexcept { return hash_value(*face.v0(), *face.v1(), *face.v2()); }

}  // namespace gfx

#endif  // SRC_GEOMETRY_FACE_H_
