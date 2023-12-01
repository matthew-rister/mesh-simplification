#ifndef SRC_GEOMETRY_VERTEX_H_
#define SRC_GEOMETRY_VERTEX_H_

#include <cassert>
#include <cstdint>
#include <memory>

#include <glm/vec3.hpp>

namespace gfx {
class HalfEdge;

class Vertex {
public:
  Vertex(const std::uint32_t id, const glm::vec3& position) noexcept : id_{id}, position_{position} {}

  [[nodiscard]] std::uint32_t id() const noexcept { return id_; }
  [[nodiscard]] const glm::vec3& position() const noexcept { return position_; }

  [[nodiscard]] std::shared_ptr<HalfEdge> edge() const noexcept {
    assert(!edge_.expired());
    return edge_.lock();
  }

  void set_edge(const std::shared_ptr<HalfEdge>& edge) noexcept { edge_ = edge; }

private:
  std::uint32_t id_;
  glm::vec3 position_;
  std::weak_ptr<HalfEdge> edge_;
};

inline std::size_t hash_value(const Vertex& v0) noexcept { return v0.id(); }

inline void hash_combine(std::size_t& seed, const Vertex& vertex) {
  seed += 0x9e3779b9 + hash_value(vertex);
  if constexpr (sizeof(std::size_t) == sizeof(std::uint32_t)) {
    seed ^= seed >> 16u;
    seed *= 0x21f0aaad;
    seed ^= seed >> 15u;
    seed *= 0x735a2d97;
    seed ^= seed >> 15u;
  } else {
    static_assert(sizeof(std::size_t) == sizeof(std::uint64_t));
    seed ^= seed >> 32u;
    seed *= 0xe9846af9b1a615d;
    seed ^= seed >> 32u;
    seed *= 0xe9846af9b1a615d;
    seed ^= seed >> 28u;
  }
}

}  // namespace gfx

#endif  // SRC_GEOMETRY_VERTEX_H_
