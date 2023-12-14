#include "geometry/face.h"

#include <array>

#include <gtest/gtest.h>

namespace {

std::array<std::shared_ptr<gfx::Vertex>, 3> CreateValidTriangle() {
  const auto v0 = std::make_shared<gfx::Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f});
  const auto v1 = std::make_shared<gfx::Vertex>(1, glm::vec3{1.0f, -1.0f, 0.0f});
  const auto v2 = std::make_shared<gfx::Vertex>(2, glm::vec3{0.0f, 0.5f, 0.0f});
  return std::array{v0, v1, v2};
}

TEST(FaceTest, InitializationOrdersVerticesByMinVertexId) {
  const auto& [v0, v1, v2] = CreateValidTriangle();
  for (const auto& face : {gfx::Face{v0, v1, v2}, gfx::Face{v1, v2, v0}, gfx::Face{v2, v0, v1}}) {
    EXPECT_EQ(face.v0(), v0);
    EXPECT_EQ(face.v1(), v1);
    EXPECT_EQ(face.v2(), v2);
  }
}

TEST(FaceTest, InitializationCalculatesTheFaceArea) {
  const auto& [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  EXPECT_FLOAT_EQ(1.5f, face012.area());
}

TEST(FaceTest, InitializationCalculatesTheFaceNormal) {
  const auto& [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  EXPECT_EQ((glm::vec3{0.0f, 0.0f, 1.0f}), face012.normal());
}

TEST(FaceTest, EqualFacesHaveTheSameHashValue) {
  const auto& [v0, v1, v2] = CreateValidTriangle();
  const gfx::Face face012{v0, v1, v2};
  const auto face012_copy = face012;  // NOLINT(performance-unnecessary-copy-initialization)
  EXPECT_EQ(face012, face012_copy);
  EXPECT_EQ(hash_value(face012), hash_value(face012_copy));
}

#ifndef NDEBUG

TEST(FaceTest, GetExpiredVertexCausesProgramExit) {
  std::unique_ptr<gfx::Face> face012;
  {
    const auto& [v0, v1, v2] = CreateValidTriangle();
    face012 = std::make_unique<gfx::Face>(v0, v1, v2);
  }
  EXPECT_DEATH({ std::ignore = face012->v0(); }, "");  // NOLINT(whitespace/newline)
  EXPECT_DEATH({ std::ignore = face012->v1(); }, "");  // NOLINT(whitespace/newline)
  EXPECT_DEATH({ std::ignore = face012->v2(); }, "");  // NOLINT(whitespace/newline)
}

TEST(FaceTest, InitializationWithCollinearVerticesCausesProgramExit) {
  const auto v0 = std::make_shared<gfx::Vertex>(0, glm::vec3{-1.0f, -1.0f, 0.0f});
  const auto v1 = std::make_shared<gfx::Vertex>(1, glm::vec3{0.0f, -1.0f, 0.0f});
  const auto v2 = std::make_shared<gfx::Vertex>(2, glm::vec3{1.0f, -1.0f, 0.0f});
  EXPECT_DEATH((gfx::Face{v0, v1, v2}), "");  // NOLINT(whitespace/newline)
}

#endif

}  // namespace
