#pragma once

#include <memory>

#include <glm/vec3.hpp>

#include "geometry/vertex.h"

namespace qem {
	class HalfEdge;

	/** \brief A triangle face defined by three vertices in counter-clockwise winding order. */
	class Face {

	public:
		/**
		 * \brief Initializes a triangle face.
		 * \param v0,v1,v2 The face vertices.
		 */
		Face(const std::shared_ptr<const Vertex>& v0,
		     const std::shared_ptr<const Vertex>& v1,
		     const std::shared_ptr<const Vertex>& v2);

		/** \brief Gets the first face vertex. */
		[[nodiscard]] std::shared_ptr<const Vertex> v0() const { return std::shared_ptr{v0_}; }

		/** \brief Gets the second face vertex. */
		[[nodiscard]] std::shared_ptr<const Vertex> v1() const { return std::shared_ptr{v1_}; }

		/** \brief Gets the third face vertex. */
		[[nodiscard]] std::shared_ptr<const Vertex> v2() const { return std::shared_ptr{v2_}; }

		/** \brief Gets the face normal. */
		[[nodiscard]] const glm::vec3& normal() const noexcept { return normal_; }

		/** \brief Gets the face area. */
		[[nodiscard]] float area() const noexcept { return area_; }

		/** \brief Gets the face hash value. */
		friend std::uint64_t hash_value(const Face& face) {
			return hash_value(*face.v0(), *face.v1(), *face.v2());
		}

	private:
		std::weak_ptr<const Vertex> v0_, v1_, v2_;
		glm::vec3 normal_;
		float area_;
	};
}
