#pragma once

#include <memory>
#include <optional>

#include <glm/vec3.hpp>

namespace qem {
	class HalfEdge;

	/** \brief A half-edge mesh vertex. */
	class Vertex {

	public:
		/**
		 * \brief Initializes a vertex.
		 * \param position The vertex position.
		 */
		explicit Vertex(const glm::vec3& position) noexcept : position_{position} {}

		/**
		 * \brief Initializes a vertex.
		 * \param id The vertex ID.
		 * \param position The vertex position.
		 */
		Vertex(const std::uint64_t id, const glm::vec3& position) noexcept : id_{id}, position_{position} {}

		/** \brief Gets the vertex ID. */
		[[nodiscard]] std::uint64_t id() const noexcept {
			assert(id_.has_value());
			return *id_;
		}

		/** \brief Sets the vertex ID. */
		void set_id(const std::uint64_t id) noexcept { id_ = id; }

		/** \brief Gets the vertex position. */
		[[nodiscard]] const glm::vec3& position() const noexcept { return position_; }

		/** \brief Gets the last created half-edge that points to this vertex. */
		[[nodiscard]] std::shared_ptr<const HalfEdge> edge() const { return std::shared_ptr{edge_}; }

		/** \brief Sets the vertex half-edge. */
		void set_edge(const std::shared_ptr<const HalfEdge>& edge) noexcept { edge_ = edge; }

		/** \brief Gets the hash value for a vertex. */
		friend std::uint64_t hash_value(const Vertex& v0) noexcept {
			assert(v0.id_.has_value());
			return std::hash<std::uint64_t>{}(*v0.id_);
		}

		/** \brief Gets the hash value for two vertices. */
		friend std::uint64_t hash_value(const Vertex& v0, const Vertex& v1) noexcept {
			std::uint64_t seed = 0x32C95994;
			seed ^= (seed << 6) + (seed >> 2) + 0x3FA612CE + hash_value(v0);
			seed ^= (seed << 6) + (seed >> 2) + 0x197685C2 + hash_value(v1);
			return seed;
		}

		/** \brief Gets the hash value for three vertices. */
		friend std::uint64_t hash_value(const Vertex& v0, const Vertex& v1, const Vertex& v2) noexcept {
			std::uint64_t seed = 0x230402B5;
			seed ^= (seed << 6) + (seed >> 2) + 0x72C2C6EB + hash_value(v0);
			seed ^= (seed << 6) + (seed >> 2) + 0x16E199E4 + hash_value(v1);
			seed ^= (seed << 6) + (seed >> 2) + 0x6F89F2A8 + hash_value(v2);
			return seed;
		}

	private:
		std::optional<std::uint64_t> id_;
		glm::vec3 position_;
		std::weak_ptr<const HalfEdge> edge_;
	};
}
