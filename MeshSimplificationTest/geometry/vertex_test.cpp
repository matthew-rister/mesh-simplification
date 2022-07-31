#include "geometry/vertex.h"

#include <format>

#include <gtest/gtest.h>

using namespace glm;
using namespace qem;
using namespace std;

namespace {

	TEST(VertexTest, TestEqualVerticesProduceTheSameHashValue) {
		const Vertex v0{0, vec3{0.f}};
		ASSERT_EQ(hash_value(v0), hash_value(Vertex{v0}));
	}

	TEST(VertexTest, TestEqualVertexPairsProduceTheSameHashValue) {
		const Vertex v0{0, vec3{0.f}};
		const Vertex v1{1, vec3{2.f}};
		ASSERT_EQ(hash_value(v0, v1), hash_value(Vertex{v0}, Vertex{v1}));
	}

	TEST(VertexTest, TestEqualVertexTriplesProduceTheSameHashValue) {
		const Vertex v0{0, vec3{0.f}};
		const Vertex v1{1, vec3{2.f}};
		const Vertex v2{2, vec3{4.f}};
		ASSERT_EQ(hash_value(v0, v1, v2), hash_value(Vertex{v0}, Vertex{v1}, Vertex{v2}));
	}
}
