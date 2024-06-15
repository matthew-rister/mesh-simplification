#ifndef GEOMETRY_HALF_EDGE_H_
#define GEOMETRY_HALF_EDGE_H_

#include <cassert>
#include <memory>

#include "geometry/face.h"
#include "geometry/vertex.h"

namespace gfx {

/** \brief A directional edge in a half-edge mesh. */
class HalfEdge {
public:
  /**
   * \brief Initializes a half-edge.
   * \param vertex The vertex the half-edge will point to.
   */
  explicit HalfEdge(const std::shared_ptr<Vertex>& vertex) noexcept : vertex_{vertex} {}

  /** \brief Gets the vertex at the head of this half-edge. */
  [[nodiscard]] std::shared_ptr<Vertex> vertex() const noexcept {
    assert(!vertex_.expired());
    return vertex_.lock();
  }

  /** \brief Gets the half-edge that shares this edge's vertices in the opposite direction. */
  [[nodiscard]] std::shared_ptr<HalfEdge> flip() const noexcept {
    assert(!flip_.expired());
    return flip_.lock();
  }

  /** \brief Sets the flip half-edge. */
  void set_flip(const std::shared_ptr<HalfEdge>& flip) noexcept { flip_ = flip; }

  /** \brief Gets the next half-edge of a triangle in counter-clockwise order. */
  [[nodiscard]] std::shared_ptr<HalfEdge> next() const noexcept {
    assert(!next_.expired());
    return next_.lock();
  }

  /** \brief Sets the next half-edge. */
  void set_next(const std::shared_ptr<HalfEdge>& next) noexcept { next_ = next; }

  /** \brief Gets the face created by three counter-clockwise \c next iterations starting from this half-edge. */
  [[nodiscard]] std::shared_ptr<Face> face() const noexcept {
    assert(!face_.expired());
    return face_.lock();
  }

  /** \brief Sets the half-edge face. */
  void set_face(const std::shared_ptr<Face>& face) noexcept { face_ = face; }

  /** \brief Defines the half-edge equality operator. */
  friend bool operator==(const HalfEdge& lhs, const HalfEdge& rhs) noexcept {
    return lhs.vertex() == rhs.vertex() && lhs.flip()->vertex() == rhs.flip()->vertex();
  }

  /** \brief Gets the half-edge hash value. */
  friend std::size_t hash_value(const HalfEdge& edge) noexcept {
    return hash_value(*edge.flip()->vertex(), *edge.vertex());
  }

private:
  std::weak_ptr<Vertex> vertex_;
  std::weak_ptr<HalfEdge> next_, flip_;
  std::weak_ptr<Face> face_;
};

}  // namespace gfx

#endif  // GEOMETRY_HALF_EDGE_H_
