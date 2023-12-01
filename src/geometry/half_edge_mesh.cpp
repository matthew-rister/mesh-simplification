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

template <typename Key, typename Value>
const Value& Get(const Key id, const std::unordered_map<Key, Value>& map) {
  const auto iterator = map.find(id);
  assert(iterator != map.end());
  return iterator->second;
}

std::shared_ptr<gfx::HalfEdge> CreateHalfEdge(const std::shared_ptr<gfx::Vertex>& v0,
                                              const std::shared_ptr<gfx::Vertex>& v1,
                                              std::unordered_map<std::size_t, std::shared_ptr<gfx::HalfEdge>>& edges) {
  const auto edge01_key = hash_value(*v0, *v1);
  const auto edge10_key = hash_value(*v1, *v0);

  // prevent the creation of duplicate edges
  if (const auto iterator = edges.find(edge01_key); iterator != edges.end()) {
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

glm::vec3 GetWeightedVertexNormal(const gfx::Vertex& v0) {
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

gfx::Mesh gfx::HalfEdgeMesh::ToMesh(const Device& device) const {
  std::vector<Mesh::Vertex> vertices;
  vertices.reserve(vertices_.size());

  std::vector<std::uint32_t> indices;
  indices.reserve(3 * faces_.size());

  std::unordered_map<std::uint32_t, std::uint32_t> index_map;
  index_map.reserve(vertices_.size());

  for (std::uint32_t index = 0; const auto& vertex : vertices_ | std::views::values) {
    vertices.push_back(Mesh::Vertex{.position = vertex->position(), .normal = GetWeightedVertexNormal(*vertex)});
    index_map.emplace(vertex->id(), index++);  // map original vertex IDs to new index positions
  }

  for (const auto& face : faces_ | std::views::values) {
    indices.push_back(Get(face->v0()->id(), index_map));
    indices.push_back(Get(face->v1()->id(), index_map));
    indices.push_back(Get(face->v2()->id(), index_map));
  }

  return Mesh{device, vertices, indices, transform_};
}
