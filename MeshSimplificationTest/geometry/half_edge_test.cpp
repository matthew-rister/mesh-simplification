#include "geometry/half_edge.h"

#include <format>

#include <gtest/gtest.h>

using namespace glm;
using namespace qem;
using namespace std;

namespace {

	class HalfEdgeTest : public testing::Test {

	protected:
		HalfEdgeTest()
			: v0_{make_shared<Vertex>(0, vec3{1.f})},
			  v1_{make_shared<Vertex>(1, vec3{2.f, 0.f, 0.f})},
			  edge01_{make_shared<HalfEdge>(v1_)},
			  edge10_{make_shared<HalfEdge>(v0_)} {
			edge01_->set_flip(edge10_);
			edge10_->set_flip(edge01_);
		}

		std::shared_ptr<Vertex> v0_, v1_;
		std::shared_ptr<HalfEdge> edge01_, edge10_;
	};

	TEST_F(HalfEdgeTest, TestEqualHalfEdgesProduceTheSameHashValue) {
		ASSERT_EQ(hash_value(*edge01_), hash_value(HalfEdge{*edge01_}));
		ASSERT_NE(hash_value(*edge01_), hash_value(*edge01_->flip()));
	}

	TEST_F(HalfEdgeTest, TestTwoVerticesProduceSameHashValueAsHalfEdge) {
		const auto v0 = edge01_->flip()->vertex();
		const auto v1 = edge01_->vertex();
		ASSERT_EQ(hash_value(*v0, *v1), hash_value(*edge01_));
	}
}
