#include "geometry/half_edge_mesh.h"

#include <vector>

#include <gtest/gtest.h>

#include "geometry/face.h"
#include "geometry/half_edge.h"
#include "geometry/vertex.h"
#include "graphics/mesh.h"
#include "tests/device.h"

namespace {

gfx::Mesh CreateValidMesh() {
  const std::vector vertices{
      gfx::Mesh::Vertex{.position = {1.0f, 0.0f, 0.0f}},   // v0
      gfx::Mesh::Vertex{.position = {2.0f, 0.0f, 0.0f}},   // v1
      gfx::Mesh::Vertex{.position = {0.5f, -1.0f, 0.0f}},  // v2
      gfx::Mesh::Vertex{.position = {1.5f, -1.0f, 0.0f}},  // v3
      gfx::Mesh::Vertex{.position = {2.5f, -1.0f, 0.0f}},  // v4
      gfx::Mesh::Vertex{.position = {3.0f, 0.0f, 0.0f}},   // v5
      gfx::Mesh::Vertex{.position = {2.5f, 1.0f, 0.0f}},   // v6
      gfx::Mesh::Vertex{.position = {1.5f, 1.0f, 0.0f}},   // v7
      gfx::Mesh::Vertex{.position = {0.5f, 1.0f, 0.0f}},   // v8
      gfx::Mesh::Vertex{.position = {0.0f, 0.0f, 0.0f}}    // v9
  };
  const std::vector indices{
      0u, 2u, 3u,  // f0
      0u, 3u, 1u,  // f1
      0u, 1u, 7u,  // f2
      0u, 7u, 8u,  // f3
      0u, 8u, 9u,  // f4
      0u, 9u, 2u,  // f5
      1u, 3u, 4u,  // f6
      1u, 4u, 5u,  // f7
      1u, 5u, 6u,  // f8
      1u, 6u, 7u   // f9
  };
  return gfx::Mesh{gfx::test::Device::Get(), vertices, indices};
}

gfx::HalfEdgeMesh CreateHalfEdgeMesh() {
  const auto mesh = CreateValidMesh();
  return gfx::HalfEdgeMesh{mesh};
}

void VerifyEdge(const std::shared_ptr<gfx::Vertex>& v0,
                const std::shared_ptr<gfx::Vertex>& v1,
                const std::unordered_map<std::size_t, std::shared_ptr<gfx::HalfEdge>>& edges) {
  const auto edge01_iterator = edges.find(hash_value(*v0, *v1));
  const auto edge10_iterator = edges.find(hash_value(*v1, *v0));
  ASSERT_NE(edge01_iterator, edges.cend());
  ASSERT_NE(edge10_iterator, edges.cend());

  const auto& edge01 = edge01_iterator->second;
  const auto& edge10 = edge10_iterator->second;

  EXPECT_EQ(v0, edge10->vertex());
  EXPECT_EQ(v1, edge01->vertex());

  EXPECT_EQ(edge01, edge10->flip());
  EXPECT_EQ(edge10, edge01->flip());

  EXPECT_EQ(edge01, edge01->flip()->flip());
  EXPECT_EQ(edge10, edge10->flip()->flip());
}

void VerifyTriangles(const gfx::HalfEdgeMesh& half_edge_mesh, const std::vector<std::uint32_t>& indices) {
  const auto& vertices = half_edge_mesh.vertices();
  const auto& edges = half_edge_mesh.edges();
  const auto& faces = half_edge_mesh.faces();

  for (std::size_t i = 0; i < indices.size(); i += 3) {
    const auto v0_iterator = vertices.find(indices[i]);
    const auto v1_iterator = vertices.find(indices[i + 1]);
    const auto v2_iterator = vertices.find(indices[i + 2]);
    ASSERT_NE(v0_iterator, vertices.cend());
    ASSERT_NE(v1_iterator, vertices.cend());
    ASSERT_NE(v2_iterator, vertices.cend());

    const auto& v0 = v0_iterator->second;
    const auto& v1 = v1_iterator->second;
    const auto& v2 = v2_iterator->second;
    VerifyEdge(v0, v1, edges);
    VerifyEdge(v1, v2, edges);
    VerifyEdge(v2, v0, edges);

    const auto edge01 = edges.at(hash_value(*v0, *v1));
    const auto edge12 = edges.at(hash_value(*v1, *v2));
    const auto edge20 = edges.at(hash_value(*v2, *v0));
    EXPECT_EQ(edge01->next(), edge12);
    EXPECT_EQ(edge12->next(), edge20);
    EXPECT_EQ(edge20->next(), edge01);

    const auto face012_iterator = faces.find(hash_value(*v0, *v1, *v2));
    ASSERT_NE(face012_iterator, faces.cend());

    const auto face012 = face012_iterator->second;
    EXPECT_EQ(edge01->face(), face012);
    EXPECT_EQ(edge12->face(), face012);
    EXPECT_EQ(edge20->face(), face012);
  }
}

TEST(HalfEdgeMeshTest, CreateHalfEdgeMeshHasCorrectVerticesEdgesFacesAndIndices) {
  const auto mesh = CreateValidMesh();
  const gfx::HalfEdgeMesh half_edge_mesh{mesh};

  EXPECT_EQ(10, half_edge_mesh.vertices().size());
  EXPECT_EQ(38, half_edge_mesh.edges().size());
  EXPECT_EQ(10, half_edge_mesh.faces().size());

  VerifyTriangles(half_edge_mesh, mesh.indices());
}

TEST(HalfEdgeMeshTest, ContractEdgeAttachesIndicentEdgesToNewVertex) {
  auto half_edge_mesh = CreateHalfEdgeMesh();
  const auto& vertices = half_edge_mesh.vertices();
  const auto& edges = half_edge_mesh.edges();

  const auto& v0 = vertices.at(0);
  const auto& v1 = vertices.at(1);
  const auto& edge01 = edges.at(hash_value(*v0, *v1));

  const auto id = static_cast<std::uint32_t>(half_edge_mesh.vertices().size());
  const auto position = (v0->position() + v1->position()) / 2.0f;
  const gfx::Vertex v_new{id, position};

  half_edge_mesh.Contract(*edge01, std::make_shared<gfx::Vertex>(v_new));

  EXPECT_EQ(9, half_edge_mesh.vertices().size());
  EXPECT_EQ(32, half_edge_mesh.edges().size());
  EXPECT_EQ(8, half_edge_mesh.faces().size());

  VerifyTriangles(half_edge_mesh, std::vector{2u, 3u,  10u,   // f0
                                              3u, 4u,  10u,   // f1
                                              4u, 5u,  10u,   // f2
                                              5u, 6u,  10u,   // f3
                                              6u, 7u,  10u,   // f4
                                              7u, 8u,  10u,   // f5
                                              8u, 9u,  10u,   // f6
                                              2u, 10u, 9u});  // f7
}

#ifndef NDEBUG

TEST(HalfEdgeMeshTest, ContractHalfEdgeWithExistingMeshVertexCausesProgramExit) {
  auto half_edge_mesh = CreateHalfEdgeMesh();
  const auto& v0 = half_edge_mesh.vertices().at(0);
  const auto& v1 = half_edge_mesh.vertices().at(1);
  const auto& edge01 = half_edge_mesh.edges().at(hash_value(*v0, *v1));
  EXPECT_DEATH(half_edge_mesh.Contract(*edge01, v0), "");
}

TEST(HalfEdgeMeshTest, ContractNonexistentHalfEdgeCausesProgramExit) {
  auto half_edge_mesh = CreateHalfEdgeMesh();
  const auto next_vertex_id = static_cast<int>(half_edge_mesh.vertices().size());
  const auto v_invalid0 = std::make_shared<gfx::Vertex>(next_vertex_id, glm::vec3{0.0f});
  const auto v_invalid1 = std::make_shared<gfx::Vertex>(next_vertex_id + 1, glm::vec3{0.0f});
  const auto edge_invalid01 = std::make_shared<gfx::HalfEdge>(v_invalid1);
  const auto edge_invalid10 = std::make_shared<gfx::HalfEdge>(v_invalid0);
  edge_invalid01->set_flip(edge_invalid10);
  EXPECT_DEATH(half_edge_mesh.Contract(*edge_invalid01, std::make_shared<gfx::Vertex>(42, glm::vec3{0.0f})), "");
}

#endif

}  // namespace
