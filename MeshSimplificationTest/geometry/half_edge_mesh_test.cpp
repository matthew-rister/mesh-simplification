#include "geometry/half_edge_mesh.cpp"

#include <vector>

#include <GL/gl3w.h>
#include <gtest/gtest.h>

using namespace glm;
using namespace qem;
using namespace std;

namespace {

	class HalfEdgeMeshTest : public testing::Test {

	protected:
		HalfEdgeMeshTest()
			: v0_{make_shared<Vertex>(0, vec3{-1.f, -1.f, 0.f})},
			  v1_{make_shared<Vertex>(1, vec3{0.f, .5f, 0.f})},
			  v2_{make_shared<Vertex>(1, vec3{1.f, -1.f, 0.f})},
			  edge01_{make_shared<HalfEdge>(v1_)},
			  edge10_{make_shared<HalfEdge>(v0_)},
			  face012_{make_shared<Face>(v0_, v1_, v2_)},
			  vertices_{
				  {hash_value(*v0_), v0_},
				  {hash_value(*v1_), v1_},
				  {hash_value(*v2_), v2_}
			  },
			  edges_{
				  {hash_value(*edge01_), edge01_},
				  {hash_value(*edge10_), edge10_}
			  },
			  faces_{
				  {hash_value(*face012_), face012_}
			  } {
			edge01_->set_flip(edge10_);
			edge10_->set_flip(edge01_);
		}

		shared_ptr<Vertex> v0_, v1_, v2_;
		shared_ptr<HalfEdge> edge01_, edge10_;
		shared_ptr<Face> face012_;
		map<uint64_t, shared_ptr<Vertex>> vertices_;
		unordered_map<uint64_t, shared_ptr<HalfEdge>> edges_;
		unordered_map<uint64_t, shared_ptr<Face>> faces_;
	};

	Mesh MakeMesh() {

		const vector<vec3> positions{
			{1.f, 0.f, 0.f},   // v0
			{2.f, 0.f, 0.f},   // v1
			{.5f, -1.f, 0.f},  // v2
			{1.5f, -1.f, 0.f}, // v3
			{2.5f, -1.f, 0.f}, // v4
			{3.f, 0.f, 0.f},   // v5
			{2.5f, 1.f, 0.f},  // v6
			{1.5f, 1.f, 0.f},  // v7
			{.5f, 1.f, 0.f},   // v8
			{0.f, 0.f, 0.f}    // v9
		};

		const vector<GLuint> indices{
			0, 2, 3, // f0
			0, 3, 1, // f1
			0, 1, 7, // f2
			0, 7, 8, // f3
			0, 8, 9, // f4
			0, 9, 2, // f5
			1, 3, 4, // f6
			1, 4, 5, // f7
			1, 5, 6, // f8
			1, 6, 7  // f9
		};

		return Mesh{positions, {}, vector(10, vec3{0.f, 0.f, 1.f}), indices};
	}

	HalfEdgeMesh MakeHalfEdgeMesh() {
		const auto mesh = MakeMesh();
		return HalfEdgeMesh{mesh};
	}

	void VerifyEdge(
		const shared_ptr<Vertex>& v0,
		const shared_ptr<Vertex>& v1,
		const unordered_map<uint64_t, shared_ptr<HalfEdge>>& edges) {

		const auto edge01_iterator = edges.find(hash_value(*v0, *v1));
		const auto edge10_iterator = edges.find(hash_value(*v1, *v0));

		ASSERT_NE(edge01_iterator, edges.end());
		ASSERT_NE(edge10_iterator, edges.end());

		const auto& edge01 = edge01_iterator->second;
		const auto& edge10 = edge10_iterator->second;

		ASSERT_EQ(v0, edge10->vertex());
		ASSERT_EQ(v1, edge01->vertex());

		ASSERT_EQ(edge01, edge10->flip());
		ASSERT_EQ(edge10, edge01->flip());

		ASSERT_EQ(edge01, edge01->flip()->flip());
		ASSERT_EQ(edge10, edge10->flip()->flip());
	}

	void VerifyTriangles(const HalfEdgeMesh& half_edge_mesh, const vector<GLuint>& indices) {
		const auto& vertices = half_edge_mesh.vertices();
		const auto& edges = half_edge_mesh.edges();
		const auto& faces = half_edge_mesh.faces();

		for (size_t i = 0; i < indices.size(); i += 3) {

			const auto v0_iterator = vertices.find(indices[i]);
			const auto v1_iterator = vertices.find(indices[i + 1]);
			const auto v2_iterator = vertices.find(indices[i + 2]);

			ASSERT_NE(v0_iterator, vertices.end());
			ASSERT_NE(v1_iterator, vertices.end());
			ASSERT_NE(v2_iterator, vertices.end());

			const auto& v0 = v0_iterator->second;
			const auto& v1 = v1_iterator->second;
			const auto& v2 = v2_iterator->second;

			VerifyEdge(v0, v1, edges);
			VerifyEdge(v1, v2, edges);
			VerifyEdge(v2, v0, edges);

			const auto edge01 = edges.at(hash_value(*v0, *v1));
			const auto edge12 = edges.at(hash_value(*v1, *v2));
			const auto edge20 = edges.at(hash_value(*v2, *v0));

			ASSERT_EQ(edge01->next(), edge12);
			ASSERT_EQ(edge12->next(), edge20);
			ASSERT_EQ(edge20->next(), edge01);

			const auto face012_iterator = faces.find(hash_value(*v0, *v1, *v2));
			ASSERT_NE(face012_iterator, faces.end());

			const auto face012 = face012_iterator->second;
			ASSERT_EQ(edge01->face(), face012);
			ASSERT_EQ(edge12->face(), face012);
			ASSERT_EQ(edge20->face(), face012);
		}
	}

	TEST_F(HalfEdgeMeshTest, TestCreateHalfEdgeMesh) {

		const auto mesh = MakeMesh();
		const HalfEdgeMesh half_edge_mesh{mesh};

		ASSERT_EQ(10, half_edge_mesh.vertices().size());
		ASSERT_EQ(38, half_edge_mesh.edges().size());
		ASSERT_EQ(10, half_edge_mesh.faces().size());

		VerifyTriangles(half_edge_mesh, mesh.indices());
	}

	TEST_F(HalfEdgeMeshTest, TestCollapseEdge) {

		auto half_edge_mesh = MakeHalfEdgeMesh();
		const auto& vertices = half_edge_mesh.vertices();
		const auto& edges = half_edge_mesh.edges();
		const auto& v0 = vertices.at(0);
		const auto& v1 = vertices.at(1);
		const auto& edge01 = edges.at(hash_value(*v0, *v1));
		const Vertex v_new{half_edge_mesh.vertices().size(), (v0->position() + v1->position()) / 2.f};

		half_edge_mesh.Contract(*edge01, make_shared<Vertex>(v_new));

		ASSERT_EQ(9, half_edge_mesh.vertices().size());
		ASSERT_EQ(32, half_edge_mesh.edges().size());
		ASSERT_EQ(8, half_edge_mesh.faces().size());

		VerifyTriangles(half_edge_mesh, {
			2, 3, 10,
			3, 4, 10,
			4, 5, 10,
			5, 6, 10,
			6, 7, 10,
			7, 8, 10,
			8, 9, 10,
			2, 10, 9
		});
	}

	TEST_F(HalfEdgeMeshTest, TestGetHalfEdge) {
		ASSERT_EQ(edge01_, GetHalfEdge(*v0_, *v1_, edges_));
		ASSERT_EQ(edge10_, GetHalfEdge(*v1_, *v0_, edges_));
	}

	TEST_F(HalfEdgeMeshTest, TestDeleteVertex) {

		DeleteVertex(*v0_, vertices_);
		ASSERT_EQ(vertices_.size(), 2);

		DeleteVertex(*v1_, vertices_);
		ASSERT_EQ(vertices_.size(), 1);

		DeleteVertex(*v2_, vertices_);
		ASSERT_TRUE(vertices_.empty());
	}

	TEST_F(HalfEdgeMeshTest, TestDeleteHalfEdge) {

		DeleteEdge(*edge01_, edges_);
		ASSERT_EQ(edges_.size(), 1);

		DeleteEdge(*edge10_, edges_);
		ASSERT_TRUE(edges_.empty());
	}

	TEST_F(HalfEdgeMeshTest, TestDeleteFace) {
		DeleteFace(*face012_, faces_);
		ASSERT_TRUE(faces_.empty());
	}

#ifdef _DEBUG

	TEST_F(HalfEdgeMeshTest, TestGetInvalidHalfEdgeCausesProgramExit) {
		ASSERT_DEATH(GetHalfEdge(*v1_, *v0_, (unordered_map<uint64_t, shared_ptr<HalfEdge>>{})), "");
	}

	TEST_F(HalfEdgeMeshTest, TestDeleteInvalidVertexCausesProgramExit) {
		map<uint64_t, shared_ptr<Vertex>> vertices;
		ASSERT_DEATH(DeleteVertex(*v0_, vertices), "");
	}

	TEST_F(HalfEdgeMeshTest, TestDeleteInvalidHalfEdgeCausesProgramExit) {
		unordered_map<uint64_t, shared_ptr<HalfEdge>> edges;
		ASSERT_DEATH(DeleteEdge(*edge01_, edges), "");
	}

	TEST_F(HalfEdgeMeshTest, TestDeleteInvalidFaceCausesProgramExit) {
		unordered_map<uint64_t, shared_ptr<Face>> faces;
		ASSERT_DEATH(DeleteFace(*face012_, faces), "");
	}

#endif
}
