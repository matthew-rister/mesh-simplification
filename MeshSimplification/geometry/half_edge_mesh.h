#pragma once

#include <glm/mat4x4.hpp>

#include <map>
#include <memory>
#include <unordered_map>

namespace qem {
	class Face;
	class HalfEdge;
	class Mesh;
	class Vertex;

	/**
	 * \brief An edge centric data structure used to represent a triangle mesh.
	 * \details A half-edge mesh is comprised of directional half-edges that refer to the next edge in a triangle in
	 *          counter-clockwise order in addition to the vertex at the head of the edge. A half-edge also provides a
	 *          pointer to its flip edge which represents the same edge in the opposite direction. Using just these
	 *          three pointers, one can effectively traverse and modify edges in a triangle mesh.
	 */
	class HalfEdgeMesh {

	public:
		/**
		 * \brief Initializes a half-edge mesh.
		 * \param mesh An indexed triangle mesh to construct the half-edge mesh from.
		 */
		explicit HalfEdgeMesh(const Mesh& mesh);

		/** \brief Defines the conversion operator back to a triangle mesh. */
		explicit operator Mesh() const;

		/** \brief Gets a mapping of mesh vertices by ID. */
		[[nodiscard]] const auto& vertices() const noexcept { return vertices_; }

		/** \brief Gets a mapping of mesh half-edges by ID. */
		[[nodiscard]] const auto& edges() const noexcept { return edges_; }

		/** \brief Gets a mapping of mesh faces by ID. */
		[[nodiscard]] const auto& faces() const noexcept { return faces_; }

		/**
		 * \brief Performs edge contraction.
		 * \details Edge contraction consists of removing an edge from the mesh by merging its two vertices into a
		 *          single vertex and updating edges incident to each endpoint to connect to that new vertex.
		 * \param edge01 The edge from vertex \c v0 to \c v1 to remove.
		 * \param v_new The new vertex to update incident edges to.
		 */
		void Contract(const HalfEdge& edge01, const std::shared_ptr<Vertex>& v_new);

	private:
		std::map<std::uint64_t, std::shared_ptr<Vertex>> vertices_;
		std::unordered_map<std::uint64_t, std::shared_ptr<HalfEdge>> edges_;
		std::unordered_map<std::uint64_t, std::shared_ptr<Face>> faces_;
		glm::mat4 model_transform_;
	};
}
