#include "geometry/mesh_simplifier.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <memory>
#include <print>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "geometry/half_edge.h"
#include "geometry/half_edge_mesh.h"
#include "geometry/vertex.h"
#include "graphics/device.h"
#include "graphics/mesh.h"

namespace {

/**
 * \brief A edge contraction candidate in a half-edge mesh.
 * \details An edge contraction candidate includes all the information necessary to process the next half-edge during
 *          the mesh simplification process.
 */
struct EdgeContraction {
  /**
   * \brief Initializes an edge contraction.
   * \param edge The edge to contract.
   * \param vertex The vertex whose position optimally preserves the original shape of the mesh after edge contraction.
   * \param quadric The error quadric for this edge contraction's vertex.
   * \param cost A metric that quantifies how much the mesh will change after this edge has been contracted.
   * \param valid Indicates if this edge contraction is valid. This is used as a workaround for priority_queue not
   *              providing a method to update an existing entry's priority. As edges are updated in the mesh,
   *              duplicated entries may be inserted in the queue and this property will be used to determine if an
   *              entry refers to the most recent edge update.
   */
  EdgeContraction(const std::shared_ptr<gfx::HalfEdge>& edge,
                  const std::shared_ptr<gfx::Vertex>& vertex,
                  const glm::mat4& quadric,
                  const float cost,
                  const bool valid = true)
      : edge{edge}, vertex{vertex}, quadric{quadric}, cost{cost}, valid{valid} {}

  std::shared_ptr<gfx::HalfEdge> edge;
  std::shared_ptr<gfx::Vertex> vertex;
  glm::mat4 quadric;
  float cost;
  bool valid;
};

/**
 * \brief Gets a canonical representation of a half-edge used to disambiguate between its flip edge.
 * \param edge01 The half-edge to disambiguate.
 * \return For two vertices connected by an edge, returns the half-edge pointing to the vertex with the smallest ID.
 */
std::shared_ptr<gfx::HalfEdge> GetMinEdge(const std::shared_ptr<gfx::HalfEdge>& edge01) {
  const auto edge10 = edge01->flip();
  return edge01->vertex()->id() < edge10->vertex()->id() ? edge01 : edge10;
}

/**
 * \brief Creates an error quadric for a given vertex.
 * \param v0 The vertex to create an error quadric for.
 * \return The error quadric for \p v0.
 */
glm::mat4 CreateErrorQuadric(const gfx::Vertex& v0) {
  glm::mat4 quadric{0.0f};
  auto edgei0 = v0.edge();
  do {
    const auto& position = v0.position();
    const auto& normal = edgei0->face()->normal();
    const glm::vec4 plane{normal, -glm::dot(position, normal)};
    quadric += glm::outerProduct(plane, plane);
    edgei0 = edgei0->next()->flip();
  } while (edgei0 != v0.edge());
  return quadric;
}

/**
 * \brief Creates an edge contraction candidate.
 * \param edge01 The edge to contract.
 * \param quadrics Error quadrics for each vertex in the half-edge mesh by vertex ID.
 * \return An edge contraction candidate that includes the vertex whose position optimally preserves the original shape
 *         and its associated cost.
 */
std::shared_ptr<EdgeContraction> CreateEdgeContraction(const std::shared_ptr<gfx::HalfEdge>& edge01,
                                                       const std::unordered_map<std::uint32_t, glm::mat4>& quadrics) {
  const auto v0 = edge01->flip()->vertex();
  const auto q0_iterator = quadrics.find(v0->id());
  assert(q0_iterator != quadrics.cend());

  const auto v1 = edge01->vertex();
  const auto q1_iterator = quadrics.find(v1->id());
  assert(q1_iterator != quadrics.cend());

  const auto& q0 = q0_iterator->second;
  const auto& q1 = q1_iterator->second;
  const auto q01 = q0 + q1;

  if (glm::determinant(q01) == 0.0f) {
    // average the edge vertices if the error quadric is not invertible
    const auto position = (v0->position() + v1->position()) / 2.0f;
    return std::make_shared<EdgeContraction>(edge01, std::make_shared<gfx::Vertex>(position), q01, 0.0f);
  }

  auto position = glm::inverse(q01) * glm::vec4{0.0f, 0.0f, 0.0f, 1.0f};
  position /= position.w;

  const auto squared_distance = glm::dot(position, q01 * position);
  return std::make_shared<EdgeContraction>(edge01, std::make_shared<gfx::Vertex>(position), q01, squared_distance);
}

/**
 * \brief Determines if the removal of an edge will cause the mesh to degenerate.
 * \param edge01 The edge to evaluate.
 * \return \c true if the removal of \p edge01 will produce a non-manifold, otherwise \c false.
 */
bool WillDegenerate(const std::shared_ptr<gfx::HalfEdge>& edge01) {
  const auto v0 = edge01->flip()->vertex();
  const auto v1_next = edge01->next()->vertex();
  const auto v0_next = edge01->flip()->next()->vertex();
  std::unordered_map<std::uint32_t, std::shared_ptr<gfx::Vertex>> neighborhood;

  for (auto iterator = edge01->next(); iterator != edge01->flip(); iterator = iterator->flip()->next()) {
    if (const auto vertex = iterator->vertex(); vertex != v0 && vertex != v1_next && vertex != v0_next) {
      neighborhood.emplace(vertex->id(), vertex);
    }
  }

  for (auto iterator = edge01->flip()->next(); iterator != edge01; iterator = iterator->flip()->next()) {
    if (const auto vertex = iterator->vertex(); neighborhood.contains(vertex->id())) {
      return true;
    }
  }

  return false;
}

}  // namespace

namespace gfx {

Mesh mesh::Simplify(const Device& device, const Mesh& mesh, const float rate) {
  if (rate < 0.0f || rate > 1.0f) {
    throw std::invalid_argument{std::format("Invalid mesh simplification rate: {}", rate)};
  }

  const auto start_time = std::chrono::high_resolution_clock::now();
  HalfEdgeMesh half_edge_mesh{mesh};

  // compute error quadrics for each vertex in the mesh
  std::unordered_map<std::uint32_t, glm::mat4> quadrics;
  for (const auto& [id, vertex] : half_edge_mesh.vertices()) {
    quadrics.emplace(id, CreateErrorQuadric(*vertex));
  }

  // use a priority queue to sort edge contraction candidates by the cost of removing each edge
  static constexpr auto kSortByMinCost = [](const auto& lhs, const auto& rhs) { return lhs->cost > rhs->cost; };
  std::priority_queue<std::shared_ptr<EdgeContraction>,
                      std::vector<std::shared_ptr<EdgeContraction>>,
                      decltype(kSortByMinCost)>
      edge_contractions{kSortByMinCost};

  // this is used to invalidate existing priority queue entries as edges are updated or removed from the mesh
  std::unordered_map<std::size_t, std::shared_ptr<EdgeContraction>> valid_edges;

  // compute edge contraction candidates for each edge in the mesh
  for (const auto& edge : half_edge_mesh.edges() | std::views::values) {
    const auto min_edge = GetMinEdge(edge);
    const auto min_edge_key = hash_value(*min_edge);

    if (!valid_edges.contains(min_edge_key)) {
      auto edge_contraction = CreateEdgeContraction(min_edge, quadrics);
      edge_contractions.push(edge_contraction);
      valid_edges.emplace(min_edge_key, std::move(edge_contraction));
    }
  }

  // stop mesh simplification if the number of triangles has been sufficiently reduced
  const auto initial_face_count = half_edge_mesh.faces().size();
  const auto is_simplified = [&, target_face_count = (1.0f - rate) * static_cast<float>(initial_face_count)] {
    const auto face_count = static_cast<float>(half_edge_mesh.faces().size());
    return edge_contractions.empty() || face_count < target_face_count;
  };

  for (auto next_vertex_id = half_edge_mesh.vertices().size(); !is_simplified(); edge_contractions.pop()) {
    const auto& edge_contraction = edge_contractions.top();
    const auto& edge01 = edge_contraction->edge;
    if (!edge_contraction->valid || WillDegenerate(edge01)) continue;

    // begin processing the next edge contraction
    const auto& v_new = edge_contraction->vertex;
    v_new->set_id(static_cast<std::uint32_t>(next_vertex_id++));
    quadrics.emplace(v_new->id(), edge_contraction->quadric);

    // invalidate entries in the priority queue that will be removed during the edge contraction
    for (const auto& vi : {edge01->flip()->vertex(), edge01->vertex()}) {
      auto edgeji = vi->edge();
      do {
        const auto min_edge = GetMinEdge(edgeji);
        if (const auto iterator = valid_edges.find(hash_value(*min_edge)); iterator != valid_edges.cend()) {
          iterator->second->valid = false;
          valid_edges.erase(iterator);
        }
        edgeji = edgeji->next()->flip();
      } while (edgeji != vi->edge());
    }

    // remove the edge from the mesh and attach incident edges to the new vertex
    half_edge_mesh.Contract(*edge01, v_new);

    // add new edge contraction candidates for edges affected by the edge contraction
    std::unordered_map<std::size_t, std::shared_ptr<HalfEdge>> visited_edges;
    const auto& vi = v_new;
    auto edgeji = vi->edge();
    do {
      const auto vj = edgeji->flip()->vertex();
      auto edgekj = vj->edge();
      do {
        const auto min_edge = GetMinEdge(edgekj);
        const auto min_edge_key = hash_value(*min_edge);
        if (!visited_edges.contains(min_edge_key)) {
          if (const auto iterator = valid_edges.find(min_edge_key); iterator != valid_edges.cend()) {
            // invalidate existing edge contraction candidate in the priority queue
            iterator->second->valid = false;
          }
          auto new_edge_contraction = CreateEdgeContraction(min_edge, quadrics);
          valid_edges[min_edge_key] = new_edge_contraction;
          edge_contractions.push(std::move(new_edge_contraction));
          visited_edges.emplace(min_edge_key, min_edge);
        }
        edgekj = edgekj->next()->flip();
      } while (edgekj != vj->edge());
      edgeji = edgeji->next()->flip();
    } while (edgeji != vi->edge());
  }

  std::println(std::clog,
               "Mesh simplified from {} to {} triangles in {} seconds",
               initial_face_count,
               half_edge_mesh.faces().size(),
               std::chrono::duration<float>{std::chrono::high_resolution_clock::now() - start_time}.count());

  return half_edge_mesh.ToMesh(device);
}

}  // namespace gfx
