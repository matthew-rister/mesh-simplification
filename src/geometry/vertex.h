#ifndef SRC_GEOMETRY_VERTEX_H_
#define SRC_GEOMETRY_VERTEX_H_

#include <cassert>
#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>

#include <glm/vec3.hpp>

namespace gfx {
class HalfEdge;

class Vertex {
public:
  explicit Vertex(const glm::vec3& position) noexcept : position_{position} {}

  Vertex(const std::uint32_t id, const glm::vec3& position) noexcept : id_{id}, position_{position} {}

  [[nodiscard]] std::uint32_t id() const noexcept {
    assert(id_.has_value());
    return *id_;
  }

  void set_id(const std::uint32_t id) noexcept { id_ = id; }

  [[nodiscard]] const glm::vec3& position() const noexcept { return position_; }

  [[nodiscard]] std::shared_ptr<HalfEdge> edge() const noexcept {
    assert(!edge_.expired());
    return edge_.lock();
  }

  void set_edge(const std::shared_ptr<HalfEdge>& edge) noexcept { edge_ = edge; }

  friend std::size_t hash_value(const std::convertible_to<Vertex> auto&... vertices) noexcept {
    std::size_t seed = 0;
    hash_combine(seed, vertices...);
    return seed;
  }

private:
  friend void hash_combine(std::size_t& seed, const Vertex& vertex, const std::convertible_to<Vertex> auto&... rest) {
    // NOLINTBEGIN(*-magic-numbers)
    seed += 0x9e3779b9 + vertex.id();
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
      // the is a workaround to enable using static_assert(false) in if constexpr expressions
      ([]<bool False = false>() { static_assert(False, "Unsupported processor architecture"); })();
    }
    (hash_combine(seed, rest), ...);
    // NOLINTEND(*-magic-numbers)
  }

  std::optional<std::uint32_t> id_;
  glm::vec3 position_;
  std::weak_ptr<HalfEdge> edge_;
};

}  // namespace gfx

#endif  // SRC_GEOMETRY_VERTEX_H_
