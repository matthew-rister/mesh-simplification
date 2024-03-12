#include "geometry/vertex.h"

#include <gtest/gtest.h>

#include "geometry/half_edge.h"

namespace {

TEST(VertexTest, EqualVerticesHaveTheSameHashValue) {
  const gfx::Vertex vertex{0, glm::vec3{0.0f}};
  const auto vertex_copy = vertex;  // NOLINT(performance-unnecessary-copy-initialization)
  EXPECT_EQ(vertex, vertex_copy);
  EXPECT_EQ(hash_value(vertex), hash_value(vertex_copy));
}

#ifndef NDEBUG

TEST(VertexTest, GetUnsetIdCausesProgramExit) {
  const gfx::Vertex vertex{glm::vec3{0.0f}};
  EXPECT_DEATH({ std::ignore = vertex.id(); }, "");  // NOLINT(whitespace/newline)
}

TEST(VertexTest, GetExpiredEdgeCausesProgramExit) {
  const auto vertex = std::make_shared<gfx::Vertex>(0, glm::vec3{0.0f});
  {
    const auto edge = std::make_shared<gfx::HalfEdge>(vertex);
    vertex->set_edge(edge);
  }
  EXPECT_DEATH({ std::ignore = vertex->edge(); }, "");  // NOLINT(whitespace/newline)
}

#endif

}  // namespace
