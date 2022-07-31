#pragma once

#include <span>
#include <vector>

#include <GL/gl3w.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qem {

	/** \brief A renderable triangle mesh. */
	class Mesh {

	public:
		/**
		 * \brief Initializes a triangle mesh.
		 * \param positions The mesh vertex positions.
		 * \param texture_coordinates The mesh texture coordinates.
		 * \param normals The mesh normals.
		 * \param indices Element indices such that each three consecutive integers define a triangle face in the mesh.
		 * \param model_transform A 4x4 matrix representing an affine transform to apply to the mesh in model space.
		 * \throw std::invalid_argument Indicates the provided arguments do not represent a valid triangle mesh.
		 * \note If \p indices is empty, \p positions must describe a triangle mesh (i.e., be a nonzero multiple of 3).
		 *       If nonempty, \p texture_coordinates and \p normals must be the same size as \p positions so that data
		 *       is aligned when sent to the vertex shader. If \p indices if nonempty, it must describe a triangle mesh,
		 *       however, \p positions, \p texture_coordinates, and \p normals may be of any size. Consequentially, each
		 *       index assumes alignment between \p positions and \p texture_coordinates, \p normals.
		 */
		explicit Mesh(
			std::span<const glm::vec3> positions,
			std::span<const glm::vec2> texture_coordinates = {},
			std::span<const glm::vec3> normals = {},
			std::span<const GLuint> indices = {},
			const glm::mat4& model_transform = glm::mat4{1.f});
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		Mesh(Mesh&& mesh) noexcept;
		Mesh& operator=(Mesh&& mesh) noexcept;

		/** \brief Gets the mesh vertex positions. */
		[[nodiscard]] const std::vector<glm::vec3>& positions() const noexcept { return positions_; }

		/** \brief Gets the mesh texture coordinates. */
		[[nodiscard]] const std::vector<glm::vec2>& texture_coordinates() const noexcept { return texture_coordinates_; }

		/** \brief Gets the mesh normals. */
		[[nodiscard]] const std::vector<glm::vec3>& normals() const noexcept { return normals_; }

		/** \brief Gets the mesh indices corresponding to a triangle face for every three consecutive integers. */
		[[nodiscard]] const std::vector<GLuint>& indices() const noexcept { return indices_; }

		/** \brief Gets the affine transform to apply to the mesh in model space. */
		[[nodiscard]] const glm::mat4& model_transform() const noexcept { return model_transform_; }

		/** \brief Renders the mesh to the current render target. */
		void Render() const noexcept {
			glBindVertexArray(vertex_array_);
			if (element_buffer_) {
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
			} else {
				glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(positions_.size()));
			}
			glBindVertexArray(0);
		}

		/**
		 * \brief Scales the mesh in local object space.
		 * \param xyz The x,y,z directions to scale the mesh.
		 */
		void Scale(const glm::vec3& xyz) {
			model_transform_ = glm::scale(model_transform_, xyz);
		}

		/**
		 * \brief Rotates the mesh in local object space.
		 * \param axis The axis to rotate the mesh about.
		 * \param angle The rotation angle specified in radians.
		 */
		void Rotate(const glm::vec3& axis, const GLfloat angle) {
			model_transform_ = glm::rotate(model_transform_, angle, axis);
		}

		/**
		 * \brief Translates the mesh in local object space.
		 * \param xyz The x,y,z directions to translate the mesh.
		 */
		void Translate(const glm::vec3& xyz) {
			model_transform_ = glm::translate(model_transform_, xyz);
		}

	private:
		GLuint vertex_array_ = 0, vertex_buffer_ = 0, element_buffer_ = 0;
		std::vector<glm::vec3> positions_;
		std::vector<glm::vec2> texture_coordinates_;
		std::vector<glm::vec3> normals_;
		std::vector<GLuint> indices_;
		glm::mat4 model_transform_;
	};
}
