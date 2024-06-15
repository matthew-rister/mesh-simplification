#ifndef GEOMETRY_FACE_H_
#define GEOMETRY_FACE_H_

#include <cassert>
#include <memory>

#include <glm/vec3.hpp>

#include "geometry/vertex.h"

namespace gfx {

/** \brief A triangle face in a half-edge mesh. */
class Face {
public:
  /**
   * \brief Initializes a face.
   * \param v0,v1,v2 The face vertices in counter-clockwise order.
   */
  Face(const std::shared_ptr<Vertex>& v0,
       const std::shared_ptr<Vertex>& v1,
       const std::shared_ptr<Vertex>& v2) noexcept;

  /** \brief Gets the first face vertex. */
  [[nodiscard]] std::shared_ptr<Vertex> v0() const noexcept {
    assert(!v0_.expired());
    return v0_.lock();
  }

  /** \brief Gets the second face vertex. */
  [[nodiscard]] std::shared_ptr<Vertex> v1() const noexcept {
    assert(!v1_.expired());
    return v1_.lock();
  }

  /** \brief Gets the third face vertex. */
  [[nodiscard]] std::shared_ptr<Vertex> v2() const noexcept {
    assert(!v2_.expired());
    return v2_.lock();
  }

  /** \brief Gets the face normal. */
  [[nodiscard]] const glm::vec3& normal() const noexcept { return normal_; }

  /** \brief Gets the face area. */
  [[nodiscard]] float area() const noexcept { return area_; }

  /** \brief Defines the face equality operator. */
  friend bool operator==(const Face& lhs, const Face& rhs) noexcept {
    return lhs.v0() == rhs.v0() && lhs.v1() == rhs.v1() && lhs.v2() == rhs.v2();
  }

  /** \brief Gets the face hash value. */
  friend std::size_t hash_value(const Face& face) noexcept { return hash_value(*face.v0(), *face.v1(), *face.v2()); }

private:
  std::weak_ptr<Vertex> v0_, v1_, v2_;
  glm::vec3 normal_;
  float area_;
};

}  // namespace gfx

#endif  // GEOMETRY_FACE_H_
