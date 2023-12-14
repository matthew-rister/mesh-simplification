#include "geometry/half_edge_mesh.h"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/vertex.h"
#include "graphics/device.h"
#include "graphics/mesh.h"

namespace {

/**
 * \brief Finds a hashable item in an unordered map.
 * \param key The hash value or item to be hashed before searching the unordered map.
 * \param map The unordered map to search.
 * \return A constant iterator to the item in the unordered map if it exists.
 */
template <typename Key, typename MapKey, typename MapValue>
typename std::unordered_map<MapKey, MapValue>::const_iterator Find(const Key& key,
                                                                   const std::unordered_map<MapKey, MapValue>& map) {
  if constexpr (std::is_same_v<Key, MapKey>) {
    return map.find(key);
  } else {
    return map.find(hash_value(key));
  }
}

/**
 * \brief Gets a hashable item in an unordered map.
 * \param key The hash value or item to be hashed before searching the unordered map.
 * \param map The unordered map to get from.
 * \return A constant reference to the item in the unordered map.
 */
template <typename Key, typename MapKey, typename MapValue>
const MapValue& Get(const Key& key, const std::unordered_map<MapKey, MapValue>& map) {
  const auto iterator = Find(key, map);
  assert(iterator != map.cend());
  return iterator->second;
}

/**
 * \brief Deletes a hashable item in an unordered map.
 * \param key The hash value or item to be hashed before searching the unordered map.
 * \param map The unordered map to delete from.
 */
template <typename Key, typename MapKey, typename MapValue>
void Delete(const Key& key, std::unordered_map<MapKey, MapValue>& map) {
  if constexpr (std::is_same_v<Key, gfx::HalfEdge>) {
    // hash values depend on both edges so they must be calculated first before deleting
    const auto edge_key = hash_value(key);
    const auto edge_flip_key = hash_value(*key.flip());
    Delete(edge_key, map);
    Delete(edge_flip_key, map);
  } else {
    const auto iterator = Find(key, map);
    assert(iterator != map.cend());
    map.erase(iterator);
  }
}

/**
 * \brief Creates a new half-edge and its associated flip edge.
 * \param v0,v1 The half-edge vertices.
 * \param edges The mesh half-edges by hash value.
 * \return The half-edge connecting vertex \p v0 to \p v1.
 */
std::shared_ptr<gfx::HalfEdge> CreateHalfEdge(const std::shared_ptr<gfx::Vertex>& v0,
                                              const std::shared_ptr<gfx::Vertex>& v1,
                                              std::unordered_map<std::size_t, std::shared_ptr<gfx::HalfEdge>>& edges) {
  const auto edge01_key = hash_value(*v0, *v1);
  const auto edge10_key = hash_value(*v1, *v0);

  // prevent the creation of duplicate edges
  if (const auto iterator = edges.find(edge01_key); iterator != edges.cend()) {
    assert(edges.contains(edge10_key));
    return iterator->second;
  }
  assert(!edges.contains(edge10_key));

  auto edge01 = std::make_shared<gfx::HalfEdge>(v1);
  auto edge10 = std::make_shared<gfx::HalfEdge>(v0);

  edge01->set_flip(edge10);
  edge10->set_flip(edge01);

  edges.emplace(edge01_key, edge01);
  edges.emplace(edge10_key, std::move(edge10));

  return edge01;
}

/**
 * \brief Creates a new triangle in the half-edge mesh.
 * \param v0,v1,v2 The triangle vertices in counter-clockwise order.
 * \param edges A mesh half-edges by hash value.
 * \return A triangle face constructed from the vertices \p v0, \p v1, \p v2.
 */
std::shared_ptr<gfx::Face> CreateTriangle(const std::shared_ptr<gfx::Vertex>& v0,
                                          const std::shared_ptr<gfx::Vertex>& v1,
                                          const std::shared_ptr<gfx::Vertex>& v2,
                                          std::unordered_map<std::size_t, std::shared_ptr<gfx::HalfEdge>>& edges) {
  const auto edge01 = CreateHalfEdge(v0, v1, edges);
  const auto edge12 = CreateHalfEdge(v1, v2, edges);
  const auto edge20 = CreateHalfEdge(v2, v0, edges);

  v0->set_edge(edge20);
  v1->set_edge(edge01);
  v2->set_edge(edge12);

  edge01->set_next(edge12);
  edge12->set_next(edge20);
  edge20->set_next(edge01);

  auto face012 = std::make_shared<gfx::Face>(v0, v1, v2);
  edge01->set_face(face012);
  edge12->set_face(face012);
  edge20->set_face(face012);

  return face012;
}

/**
 * \brief Attaches edges incident to a vertex to a new vertex.
 * \param v_target The vertex whose incident edges should be updated.
 * \param v_start The vertex opposite of \p v_target representing the first half-edge to process.
 * \param v_end The vertex opposite of \p v_target representing the last half-edge to process.
 * \param v_new The new vertex to attach edges to.
 * \param edges The mesh half-edges by hash value.
 * \param faces The mesh faces by hash value.
 */
void AttachIncidentEdges(const gfx::Vertex& v_target,
                         const gfx::Vertex& v_start,
                         const gfx::Vertex& v_end,
                         const std::shared_ptr<gfx::Vertex>& v_new,
                         std::unordered_map<std::size_t, std::shared_ptr<gfx::HalfEdge>>& edges,
                         std::unordered_map<std::size_t, std::shared_ptr<gfx::Face>>& faces) {
  const auto& edge_start = Get(hash_value(v_target, v_start), edges);
  const auto& edge_end = Get(hash_value(v_target, v_end), edges);

  for (auto edge0i = edge_start; edge0i != edge_end;) {
    const auto edgeij = edge0i->next();
    const auto edgej0 = edgeij->next();

    const auto vi = edge0i->vertex();
    const auto vj = edgeij->vertex();

    auto face_new = CreateTriangle(v_new, vi, vj, edges);
    faces.emplace(hash_value(*face_new), std::move(face_new));

    Delete(*edge0i->face(), faces);
    Delete(*edge0i, edges);

    edge0i = edgej0->flip();
  }

  Delete(*edge_end, edges);
}

/**
 * \brief Computes a vertex normal by averaging its face normals weighted by surface area.
 * \param v0 The vertex to compute the normal for.
 * \return The weighted vertex normal.
 */
glm::vec3 AverageVertexNormals(const gfx::Vertex& v0) {
  glm::vec3 normal{0.0f};
  auto edgei0 = v0.edge();
  do {
    const auto& face = edgei0->face();
    normal += face->normal() * face->area();
    edgei0 = edgei0->next()->flip();
  } while (edgei0 != v0.edge());
  return glm::normalize(normal);
}

}  // namespace

gfx::HalfEdgeMesh::HalfEdgeMesh(const Mesh& mesh)
    : vertices_{mesh.vertices()  //
                | std::views::transform([id = 0u](const auto& mesh_vertex) mutable {
                    auto vertex = std::make_shared<Vertex>(id, mesh_vertex.position);
                    return std::pair{id++, std::move(vertex)};
                  })
                | std::ranges::to<std::unordered_map>()},
      faces_{mesh.indices()          //
             | std::views::chunk(3)  //
             | std::views::transform([this](const auto& index_group) {
                 const auto& v0 = Get(index_group[0], vertices_);
                 const auto& v1 = Get(index_group[1], vertices_);
                 const auto& v2 = Get(index_group[2], vertices_);
                 auto face012 = CreateTriangle(v0, v1, v2, edges_);
                 return std::pair{hash_value(*face012), std::move(face012)};
               })
             | std::ranges::to<std::unordered_map>()},
      transform_{mesh.transform()} {}

void gfx::HalfEdgeMesh::Contract(const HalfEdge& edge01, const std::shared_ptr<Vertex>& v_new) {
  assert(Find(edge01, edges_) != edges_.cend());
  assert(Find(v_new->id(), vertices_) == vertices_.cend());

  const auto edge10 = edge01.flip();
  const auto v0 = edge10->vertex();
  const auto v1 = edge01.vertex();
  const auto v0_next = edge10->next()->vertex();
  const auto v1_next = edge01.next()->vertex();

  AttachIncidentEdges(*v0, *v1_next, *v0_next, v_new, edges_, faces_);
  AttachIncidentEdges(*v1, *v0_next, *v1_next, v_new, edges_, faces_);

  Delete(*edge01.face(), faces_);
  Delete(*edge10->face(), faces_);

  Delete(edge01, edges_);

  Delete(v0->id(), vertices_);
  Delete(v1->id(), vertices_);

  vertices_.emplace(v_new->id(), v_new);
}

gfx::Mesh gfx::HalfEdgeMesh::ToMesh(const Device& device) const {
  std::vector<Mesh::Vertex> vertices;
  vertices.reserve(vertices_.size());

  std::vector<std::uint32_t> indices;
  indices.reserve(3 * faces_.size());

  std::unordered_map<std::uint32_t, std::uint32_t> index_map;
  index_map.reserve(vertices_.size());

  for (std::uint32_t index = 0; const auto& vertex : vertices_ | std::views::values) {
    vertices.push_back(Mesh::Vertex{.position = vertex->position(), .normal = AverageVertexNormals(*vertex)});
    index_map.emplace(vertex->id(), index++);  // map original vertex IDs to new index positions
  }

  for (const auto& face : faces_ | std::views::values) {
    indices.push_back(Get(face->v0()->id(), index_map));
    indices.push_back(Get(face->v1()->id(), index_map));
    indices.push_back(Get(face->v2()->id(), index_map));
  }

  return Mesh{device, vertices, indices, transform_};
}
