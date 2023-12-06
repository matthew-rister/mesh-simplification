#ifndef SRC_GEOMETRY_HALF_EDGE_H_
#define SRC_GEOMETRY_HALF_EDGE_H_

#include <cassert>
#include <memory>

#include "geometry/face.h"
#include "geometry/vertex.h"

namespace gfx {

class HalfEdge {
public:
  explicit HalfEdge(const std::shared_ptr<Vertex>& vertex) noexcept : vertex_{vertex} {}

  [[nodiscard]] std::shared_ptr<Vertex> vertex() const noexcept {
    assert(!vertex_.expired());
    return vertex_.lock();
  }

  [[nodiscard]] std::shared_ptr<HalfEdge> flip() const noexcept {
    assert(!flip_.expired());
    return flip_.lock();
  }

  void set_flip(const std::shared_ptr<HalfEdge>& flip) noexcept { flip_ = flip; }

  [[nodiscard]] std::shared_ptr<HalfEdge> next() const noexcept {
    assert(!next_.expired());
    return next_.lock();
  }

  void set_next(const std::shared_ptr<HalfEdge>& next) noexcept { next_ = next; }

  [[nodiscard]] std::shared_ptr<Face> face() const noexcept {
    assert(!face_.expired());
    return face_.lock();
  }

  void set_face(const std::shared_ptr<Face>& face) noexcept { face_ = face; }

  friend std::size_t hash_value(const HalfEdge& edge) noexcept {
    return hash_value(*edge.flip()->vertex(), *edge.vertex());
  }

private:
  std::weak_ptr<Vertex> vertex_;
  std::weak_ptr<HalfEdge> next_, flip_;
  std::weak_ptr<Face> face_;
};

}  // namespace gfx

#endif  // SRC_GEOMETRY_HALF_EDGE_H_
