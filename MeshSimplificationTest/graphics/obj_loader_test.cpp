#include "graphics/obj_loader.cpp"

#include <glm/glm.hpp>
#include <gtest/gtest.h>

using namespace glm;
using namespace qem;
using namespace std;

namespace {

	TEST(StringTest, TestTrimWhitespaceString) {
		constexpr auto* line = "     ";
		static_assert(Trim(line).empty());
	}

	TEST(StringTest, TestTrimString) {
		constexpr auto* line = "\t  Hello, World!  \t";
		static_assert("Hello, World!" == Trim(line));
	}

	TEST(StringTest, TestSplitEmptyString) {
		constexpr auto* line = "";
		const auto tokens = Split(line, " ");
		ASSERT_TRUE(tokens.empty());
	}

	TEST(StringTest, TestSplitWhitespaceString) {
		constexpr auto* line = "   ";
		const auto tokens = Split(line, " ");
		ASSERT_TRUE(tokens.empty());
	}

	TEST(StringTest, TestSplitNoWhitespaceString) {
		constexpr auto* line = "Hello";
		const auto tokens = Split(line, " ");
		ASSERT_EQ((vector<string_view>{line}), tokens);
	}

	TEST(StringTest, TestSplitStringOnWhitespaceAndTab) {
		constexpr auto* line = "\t vt 0.707 0.395 0.684 ";
		const auto tokens = Split(line, " \t");
		ASSERT_EQ((vector<string_view>{"vt", "0.707", "0.395", "0.684"}), tokens);
	}

	TEST(ObjLoaderTest, TestParseEmptyToken) {
		ASSERT_THROW(ParseToken<GLint>(""), invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseInvalidToken) {
		ASSERT_THROW(ParseToken<GLfloat>("Definitely a float"), invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseIntToken) {
		ASSERT_EQ(42, ParseToken<GLint>("42"));
	}

	TEST(ObjLoaderTest, TestParseFloatToken) {
		ASSERT_FLOAT_EQ(3.14f, ParseToken<GLfloat>("3.14"));
	}

	TEST(ObjLoaderTest, TestParseEmptyLine) {
		ASSERT_THROW((ParseLine<GLfloat, 3>("")), invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseLineWithInvalidSizeArgument) {
		ASSERT_THROW((ParseLine<GLfloat, 2>("vt 0.707 0.395 0.684")), invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseLine) {
		ASSERT_EQ((vec3{.707f, .395f, .684f}), (ParseLine<GLfloat, 3>("vt 0.707 0.395 0.684")));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionIndex) {
		ASSERT_EQ((ivec3{0, kInvalidFaceElementIndex, kInvalidFaceElementIndex}), ParseIndexGroup("1"));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionAndTextureCoordinatesIndices) {
		ASSERT_EQ((ivec3{0, 1, kInvalidFaceElementIndex}), ParseIndexGroup("1/2"));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionAndNormalIndices) {
		ASSERT_EQ((ivec3{0, kInvalidFaceElementIndex, 1}), ParseIndexGroup("1//2"));
	}

	TEST(ObjLoaderTest, TestParseIndexGroupWithPositionTextureCoordinateAndNormalIndices) {
		ASSERT_EQ((ivec3{0, 1, 2}), ParseIndexGroup("1/2/3"));
	}

	TEST(ObjLoaderTest, TestParseInvalidIndexGroup) {
		ASSERT_THROW(ParseIndexGroup(""), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("//"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1/"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1//"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2/"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("//3"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("1/2/"), invalid_argument);
		ASSERT_THROW(ParseIndexGroup("/2/3"), invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseFaceWithInvalidNumberOfIndexGroups) {
		ASSERT_THROW(ParseFace("f 1/2/3 4/5/6"), invalid_argument);
		ASSERT_THROW(ParseFace("f 1/2/3 4/5/6 7/8/9 10/11/12"), invalid_argument);
	}

	TEST(ObjLoaderTest, TestParseFaceWithThreeIndexGroups) {
		ASSERT_EQ((array{
			          ivec3{0, 1, 2},
			          ivec3{3, 4, 5},
			          ivec3{6, 7, 8}
			          }), ParseFace("f 1/2/3 4/5/6 7/8/9"));
	}

	TEST(ObjLoaderTest, TestLoadMeshWithoutFaceIndices) {

		istringstream ss{
			R"(
			# positions
			v 0.0 0.1 0.2
			v 1.0 1.1 1.2
			v 2.0 2.1 2.2
			# texture coordinates
			vt 3.0 3.1
			vt 4.0 4.1
			vt 5.0 5.1
			# normals
			vn 6.0 6.1 6.2
			vn 7.0 7.1 7.2
			vn 8.0 8.1 8.2
		)"
		};

		const auto mesh = LoadMesh(ss);
		constexpr vec3 v0{0.f, 0.1f, 0.2f}, v1{1.f, 1.1f, 1.2f}, v2{2.f, 2.1f, 2.2f};
		constexpr vec2 vt0{3.f, 3.1f}, vt1{4.f, 4.1f}, vt2{5.f, 5.1f};
		constexpr vec3 vn0{6.f, 6.1f, 6.2f}, vn1{7.f, 7.1f, 7.2f}, vn2{8.f, 8.1f, 8.2f};

		ASSERT_EQ((vector{v0, v1, v2}), mesh.positions());
		ASSERT_EQ((vector{vt0, vt1, vt2}), mesh.texture_coordinates());
		ASSERT_EQ((vector{vn0, vn1, vn2}), mesh.normals());
		ASSERT_TRUE(mesh.indices().empty());
	}

	TEST(ObjLoaderTest, TestLoadMeshWithFaceIndices) {

		istringstream ss{
			R"(
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
			vn 8.0  8.1  8.2
			vn 9.0  9.1  9.2
			vn 10.0 10.1 10.2
			# faces
			f 1/4/2 2/1/3 3/2/1
			f 1/2/2 2/1/3 4/3/1
		)"
		};

		const auto mesh = LoadMesh(ss);
		constexpr vec3 v0{0.f, .1f, .2f}, v1{1.f, 1.1f, 1.2f}, v2{2.f, 2.1f, 2.2f}, v3{3.f, 3.1f, 3.2f};
		constexpr vec2 vt0{4.f, 4.1f}, vt1{5.f, 5.1f}, vt2{6.f, 6.1f}, vt3{7.f, 7.1f};
		constexpr vec3 vn0{8.f, 8.1f, 8.2f}, vn1{9.f, 9.1f, 9.2f}, vn2{10.f, 10.1f, 10.2f};

		ASSERT_EQ((vector{v0, v1, v2, v0, v3}), mesh.positions());
		ASSERT_EQ((vector{vt3, vt0, vt1, vt1, vt2}), mesh.texture_coordinates());
		ASSERT_EQ((vector{vn1, vn2, vn0, vn1, vn0}), mesh.normals());
		ASSERT_EQ((vector{0u, 1u, 2u, 3u, 1u, 4u}), mesh.indices());
	}
}
