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

/**
 * \brief An edge centric data structure used to represent a triangle mesh.
 * \details A half-edge mesh is comprised of directional half-edges that refer to the next edge in the triangle in
 *          counter-clockwise order. Each half-edge also provides pointers to the vertex at the head of the edge, its
 *          associated triangle face, and its flip edge which represents the same edge in the opposite direction. Using
 *          just these four pointers, one can effectively traverse and modify edges in a triangle mesh.
 */
class HalfEdgeMesh {
public:
  /**
   * \brief Initializes a half-edge mesh.
   * \param mesh An indexed triangle mesh to construct the half-edge mesh from.
   */
  explicit HalfEdgeMesh(const Mesh& mesh);

  /** \brief Gets the mesh vertices by ID. */
  [[nodiscard]] const auto& vertices() const noexcept { return vertices_; }

  /** \brief Gets the mesh half-edges by hash value. */
  [[nodiscard]] const auto& edges() const noexcept { return edges_; }

  /** \brief Gets the mesh faces by hash value. */
  [[nodiscard]] const auto& faces() const noexcept { return faces_; }

  /**
   * \brief Performs edge contraction.
   * \details Edge contraction consists of removing an edge from the mesh by merging its two vertices into a single
   *          vertex and updating edges incident to each endpoint to connect to that new vertex.
   * \param edge01 The edge from vertex \c v0 to \c v1 to remove.
   * \param v_new The new vertex to attach edges incident to \c v0 and \c v1 to.
   */
  void Contract(const HalfEdge& edge01, const std::shared_ptr<Vertex>& v_new);

  /**
   * \brief Converts the half-edge mesh back to an indexed triangle mesh.
   * \param device The graphics device used to load mesh data into GPU memory.
   * \return An indexed triangle mesh.
   */
  [[nodiscard]] Mesh ToMesh(const Device& device) const;

private:
  std::unordered_map<std::uint32_t, std::shared_ptr<Vertex>> vertices_;
  std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> edges_;
  std::unordered_map<std::size_t, std::shared_ptr<Face>> faces_;
  glm::mat4 transform_;
};

}  // namespace gfx

#endif  // SRC_GEOMETRY_HALF_EDGE_MESH_H_
