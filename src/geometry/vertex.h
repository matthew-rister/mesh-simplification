#ifndef GEOMETRY_VERTEX_H_
#define GEOMETRY_VERTEX_H_

#include <cassert>
#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>

#include <glm/vec3.hpp>

namespace gfx {
class HalfEdge;

/** \brief A vertex in a half-edge mesh. */
class Vertex {
public:
  /**
   * \brief Initializes a vertex.
   * \param position The vertex position.
   */
  explicit Vertex(const glm::vec3& position) noexcept : position_{position} {}

  /**
   * \brief Initializes a vertex.
   * \param id The vertex ID.
   * \param position The vertex position.
   */
  Vertex(const std::uint32_t id, const glm::vec3& position) noexcept : maybe_id_{id}, position_{position} {}

  /** \brief Gets the vertex ID. */
  [[nodiscard]] std::uint32_t id() const noexcept {
    assert(maybe_id_.has_value());
    return *maybe_id_;
  }

  /** \brief Sets the vertex ID. */
  void set_id(const std::uint32_t id) noexcept { maybe_id_ = id; }

  /** \brief Gets the vertex position. */
  [[nodiscard]] const glm::vec3& position() const noexcept { return position_; }

  /** \brief Gets the last created half-edge that points to this vertex. */
  [[nodiscard]] std::shared_ptr<HalfEdge> edge() const noexcept {
    assert(!edge_.expired());
    return edge_.lock();
  }

  /** \brief Sets the vertex half-edge. */
  void set_edge(const std::shared_ptr<HalfEdge>& edge) noexcept { edge_ = edge; }

  /** \brief Defines the vertex equality operator. */
  friend bool operator==(const Vertex& lhs, const Vertex& rhs) noexcept { return lhs.id() == rhs.id(); }

  /** \brief Gets the vertex hash value. */
  friend std::size_t hash_value(const Vertex& vertex) noexcept { return vertex.id(); }

  /** \brief Gets the combined hash value for an arbitrary number of vertices. */
  friend std::size_t hash_value(const std::convertible_to<Vertex> auto&... vertices) noexcept {
    std::size_t seed = 0;
    hash_combine(seed, vertices...);
    return seed;
  }

private:
  /**
   * \brief Combines the hash values of an arbitrary number of vertices.
   * \param seed The starting seed to generate hash values from.
   * \param vertex The current vertex to get the hash value for.
   * \param rest The remaining vertices to combine hash values for.
   * \remark This hash algorithm is based on boost::hash_combine.
   * \see https://www.boost.org/doc/libs/1_83_0/libs/container_hash/doc/html/hash.html#notes_hash_combine
   */
  static void hash_combine(std::size_t& seed, const Vertex& vertex, const std::convertible_to<Vertex> auto&... rest) {
    // NOLINTBEGIN(*-magic-numbers)
    seed += 0x9e3779b9 + hash_value(vertex);
    if constexpr (sizeof(std::size_t) == sizeof(std::uint32_t)) {  // x86
      seed ^= seed >> 16u;
      seed *= 0x21f0aaad;
      seed ^= seed >> 15u;
      seed *= 0x735a2d97;
      seed ^= seed >> 15u;
    } else if constexpr (sizeof(std::size_t) == sizeof(std::uint64_t)) {  // x64
      seed ^= seed >> 32u;
      seed *= 0xe9846af9b1a615d;
      seed ^= seed >> 32u;
      seed *= 0xe9846af9b1a615d;
      seed ^= seed >> 28u;
    } else {
      // this is a workaround to enable using static_assert(false) in constexpr expressions
      ([]<bool False = false>() { static_assert(False, "Unsupported processor architecture"); })();
    }
    // NOLINTEND(*-magic-numbers)
    (hash_combine(seed, rest), ...);
  }

  std::optional<std::uint32_t> maybe_id_;
  glm::vec3 position_;
  std::weak_ptr<HalfEdge> edge_;
};

}  // namespace gfx

#endif  // GEOMETRY_VERTEX_H_
