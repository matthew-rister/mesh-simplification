#include "graphics/obj_loader.cpp"  // NOLINT(build/include)

#include <ranges>

#include <gtest/gtest.h>

#include "tests/device.h"

namespace {

TEST(ObjLoaderTest, TrimStringWithOnlyWhitespaceReturnsTheEmptyString) {
  static constexpr auto* kLine = "     ";
  static_assert(Trim(kLine).empty());
}

TEST(ObjLoaderTest, TrimStringRemovesWhitespaceFromBothEndsOfTheString) {
  static constexpr auto* kLine = "  Hello, World!  ";
  static_assert(Trim(kLine) == "Hello, World!");
}

TEST(ObjLoaderTest, SplitEmptyStringReturnsAnEmptyList) {
  static constexpr auto* kLine = "";
  static constexpr auto* kDelimiter = " ";
  EXPECT_TRUE(Split(kLine, kDelimiter).empty());
}

TEST(ObjLoaderTest, SplitStringWithOnlyTheDelimiterReturnsAnEmptyList) {
  static constexpr auto* kLine = "   ";
  static constexpr auto* kDelimiter = " ";
  EXPECT_TRUE(Split(kLine, kDelimiter).empty());
}

TEST(ObjLoaderTest, SplitStringWithoutDelimiterReturnsListWithTheOriginalString) {
  static constexpr auto* kLine = "Hello";
  static constexpr auto* kDelimiter = " ";
  EXPECT_EQ(Split(kLine, kDelimiter), (std::vector<std::string_view>{kLine}));
}

TEST(ObjLoaderTest, SplitStringWithDelimiterReturnsListWithSplitStringTokens) {
  static constexpr auto* kLine = " v  0.707 0.395    0.684 ";
  static constexpr auto* kDelimiter = " ";
  EXPECT_EQ(Split(kLine, kDelimiter), (std::vector<std::string_view>{"v", "0.707", "0.395", "0.684"}));
}

TEST(ObjLoaderTest, ParseEmptyStringThrowsAnException) {  //
  EXPECT_THROW(ParseToken<int>(""), std::invalid_argument);
}

TEST(ObjLoaderTest, ParseInvalidTokenThrowsAnException) {
  EXPECT_THROW(ParseToken<float>("Definitely a float"), std::invalid_argument);
}

TEST(ObjLoaderTest, ParseIntTokenReturnsTheCorrectValue) {  //
  static_assert(ParseToken<int>("42") == 42);
}

TEST(ObjLoaderTest, ParseFloatTokenReturnsTheCorrectValue) {  //
  EXPECT_FLOAT_EQ(ParseToken<float>("3.14"), 3.14f);
}

TEST(ObjLoaderTest, ParseEmptyLineThrowsAnException) {  //
  EXPECT_THROW((ParseLine<float, 3>("")), std::invalid_argument);
}

TEST(ObjLoaderTest, ParseLineWithInvalidSizeArgumentThrowsAnException) {
  EXPECT_THROW((ParseLine<float, 2>("v 0.707 0.395 0.684")), std::invalid_argument);
}

TEST(ObjLoaderTest, ParseLineReturnsVectorWithCorrectValues) {
  EXPECT_EQ((ParseLine<float, 3>("v 0.707 0.395 0.684")), (glm::vec3{.707f, .395f, .684f}));
}

TEST(ObjLoaderTest, ParseIndexGroupWithOnlyPositionIndexReturnsCorrectIndexGroup) {
  EXPECT_EQ(ParseIndexGroup("1"), (glm::ivec3{0, kInvalidIndex, kInvalidIndex}));
}

TEST(ObjLoaderTest, ParseIndexGroupWithPositionAndTextureCoordinatesIndicesReturnsCorrectIndexGroup) {
  EXPECT_EQ(ParseIndexGroup("1/2"), (glm::ivec3{0, 1, kInvalidIndex}));
}

TEST(ObjLoaderTest, ParseIndexGroupWithPositionAndNormalIndicesReturnsCorrectIndexGroup) {
  EXPECT_EQ(ParseIndexGroup("1//2"), (glm::ivec3{0, kInvalidIndex, 1}));
}

TEST(ObjLoaderTest, ParseIndexGroupWithPositionTextureCoordinateAndNormalIndicesReturnsCorrectIndexGroup) {
  EXPECT_EQ(ParseIndexGroup("1/2/3"), (glm::ivec3{0, 1, 2}));
}

TEST(ObjLoaderTest, ParseInvalidIndexGroupThrowsAnException) {
  EXPECT_THROW(ParseIndexGroup(""), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("//"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("1/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/2"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("1//"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/2/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("//3"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("1/2/"), std::invalid_argument);
  EXPECT_THROW(ParseIndexGroup("/2/3"), std::invalid_argument);
}

TEST(ObjLoaderTest, ParseFaceWithInvalidNumberOfIndexGroupsThrowsAnException) {
  EXPECT_THROW(ParseFace("f 1/2/3"), std::invalid_argument);
  EXPECT_THROW(ParseFace("f 1/2/3 4/5/6"), std::invalid_argument);
  EXPECT_THROW(ParseFace("f 1/2/3 4/5/6 7/8/9 10/11/12"), std::invalid_argument);
}

TEST(ObjLoaderTest, ParseFaceReturnsCorrectIndexGroups) {
  static constexpr std::array kIndexGroups{glm::ivec3{0, 1, 2}, glm::ivec3{3, 4, 5}, glm::ivec3{6, 7, 8}};
  EXPECT_EQ(ParseFace("f 1/2/3 4/5/6 7/8/9"), kIndexGroups);
}

TEST(ObjLoaderTest, LoadIndexMeshGetsTheCorrectVerticesAndIndices) {
  // clang-format off
   std::istringstream istream{R"(
     # positions
     v 0.0 0.1 0.2
     v 1.0 1.1 1.2
     v 2.0 2.1 2.2
     v 3.0 3.1 3.2
     # texture coordinates
     vt 4.0 4.1
     vt 5.0 5.1
     vt 6.0 6.1
     vt 7.0 7.1
     # normals
     vn 1.0 0.0 0.0
     vn 0.0 1.0 0.0
     vn 0.0 0.0 1.0
     # faces
     f 1/4/2 2/1/3 3/2/1
     f 1/2/2 2/1/3 4/3/1
   )"};
  // clang-format on

  static constexpr glm::vec3 kV1{0.0f, 0.1f, 0.2f};
  static constexpr glm::vec3 kV2{1.0f, 1.1f, 1.2f};
  static constexpr glm::vec3 kV3{2.0f, 2.1f, 2.2f};
  static constexpr glm::vec3 kV4{3.0f, 3.1f, 3.2f};

  static constexpr glm::vec2 kVt1{4.0f, 4.1f};
  static constexpr glm::vec2 kVt2{5.0f, 5.1f};
  static constexpr glm::vec2 kVt3{6.0f, 6.1f};
  static constexpr glm::vec2 kVt4{7.0f, 7.1f};

  static constexpr glm::vec3 kVn1{1.0f, 0.0f, 0.0f};
  static constexpr glm::vec3 kVn2{0.0f, 1.0f, 0.0f};
  static constexpr glm::vec3 kVn3{0.0f, 0.0f, 1.0f};

  const auto mesh = LoadMesh(gfx::test::Device::Get(), istream);
  for (const auto& [actual_vertex, expected_vertex] :
       std::views::zip(mesh.vertices(),
                       std::vector{gfx::Mesh::Vertex{.position = kV1, .texture_coordinates = kVt4, .normal = kVn2},
                                   gfx::Mesh::Vertex{.position = kV2, .texture_coordinates = kVt1, .normal = kVn3},
                                   gfx::Mesh::Vertex{.position = kV3, .texture_coordinates = kVt2, .normal = kVn1},
                                   gfx::Mesh::Vertex{.position = kV1, .texture_coordinates = kVt2, .normal = kVn2},
                                   gfx::Mesh::Vertex{.position = kV4, .texture_coordinates = kVt3, .normal = kVn1}})) {
    EXPECT_EQ(actual_vertex.position, expected_vertex.position);
    EXPECT_EQ(actual_vertex.texture_coordinates, expected_vertex.texture_coordinates);
    EXPECT_EQ(actual_vertex.normal, expected_vertex.normal);
  }
  EXPECT_EQ(mesh.indices(), (std::vector{0u, 1u, 2u, 3u, 1u, 4u}));
}

}  // namespace
