#ifndef SRC_GEOMETRY_HALF_EDGE_MESH_H_
#define SRC_GEOMETRY_HALF_EDGE_MESH_H_

#include <memory>
#include <unordered_map>

#include <glm/mat4x4.hpp>

namespace gfx {
class Device;
class Face;
class HalfEdge;
class Mesh;
class Vertex;

class HalfEdgeMesh {
public:
  explicit HalfEdgeMesh(const Mesh& mesh);

  [[nodiscard]] Mesh ToMesh(const Device& device) const;

private:
  std::unordered_map<std::uint32_t, std::shared_ptr<Vertex>> vertices_;
  std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
  std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
  glm::mat4 transform_;
};

}  // namespace gfx

#endif  // SRC_GEOMETRY_HALF_EDGE_MESH_H_
