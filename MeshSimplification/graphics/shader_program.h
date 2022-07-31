#pragma once

#include <concepts>
#include <format>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>

namespace qem {

	/** \brief A program used to execute one or more OpenGL shaders on the host GPU. */
	class ShaderProgram {

		/** \brief A shader in the OpenGL graphics pipeline. */
		class Shader {

		public:
			/**
			 * \brief Initializes a shader.
			 * \param shader_type The shader type (e.g., GL_FRAGMENT_SHADER)
			 * \param shader_source The shader source code to be compiled.
			 * \throw std::runtime_error Indicates shader creation was unsuccessful.
			 */
			Shader(GLenum shader_type, const GLchar* shader_source);

			~Shader() { glDeleteShader(id); }

			Shader(const Shader&) = delete;
			Shader& operator=(const Shader&) = delete;

			Shader(Shader&&) noexcept = delete;
			Shader& operator=(Shader&&) noexcept = delete;

			const GLuint id;
		};

	public:
		/**
		 * \brief Initializes a shader program.
		 * \param vertex_shader_filepath The filepath to the vertex shader to be compiled.
		 * \param fragment_shader_filepath The filepath to the fragment shader to be compiled.
		 * \throw std::runtime_error Indicates the shader program creation was unsuccessful.
		 */
		ShaderProgram(std::string_view vertex_shader_filepath, std::string_view fragment_shader_filepath);

		~ShaderProgram() { glDeleteProgram(id_); }

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		ShaderProgram(ShaderProgram&&) noexcept = delete;
		ShaderProgram& operator=(ShaderProgram&&) noexcept = delete;

		/** \brief Binds the shader program to the current OpenGL context. */
		void Enable() const noexcept { glUseProgram(id_); }

		/**
		 * \brief Sets a uniform variable in the shader program.
		 * \tparam T The uniform variable type.
		 * \param name The uniform variable name.
		 * \param value The uniform variable value.
		 */
		template <typename T>
		void SetUniform(const std::string_view name, const T& value) {
			const auto location = GetUniformLocation(name);

			if constexpr (std::integral<T>) {
				glUniform1i(location, static_cast<GLint>(value));
			} else if constexpr (std::floating_point<T>) {
				glUniform1f(location, static_cast<GLfloat>(value));
			} else if constexpr (std::same_as<T, glm::vec3>) {
				glUniform3fv(location, 1, glm::value_ptr(value));
			} else if constexpr (std::same_as<T, glm::vec4>) {
				glUniform4fv(location, 1, glm::value_ptr(value));
			} else if constexpr (std::same_as<T, glm::mat3>) {
				glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
			} else if constexpr (std::same_as<T, glm::mat4>) {
				glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
			} else {
				static_assert(assert_false<T>, "Unsupported uniform variable type");
			}
		}

	private:
		// this is required as a workaround to ensure that static assertions in if constexpr statement are well-formed
		template <typename> static constexpr std::false_type assert_false{};

		// The following is needed to perform heterogeneous lookup in unordered containers. This is important because
		// each uniform location query is performed using a string_view, but stored as a string. Without heterogeneous
		// lookup, each query would have to be converted to a string (and hence allocate unnecessary memory) which would
		// degrade performance on the critical rendering path.
		struct string_view_hash {
			using is_transparent = void;

			std::size_t operator()(const std::string_view value) const noexcept {
				return std::hash<std::string_view>{}(value);
			}
		};

		/**
		 * \brief Gets the location for a uniform variable in the shader program.
		 * \param name The uniform variable name.
		 * \return An integer representing the uniform variable location. Returns -1 if the variable is not active.
		 */
		[[nodiscard]] GLint GetUniformLocation(const std::string_view name) {
			if (const auto iterator = uniform_locations_.find(name); iterator == uniform_locations_.end()) {
				const auto location = glGetUniformLocation(id_, name.data());
				if (location == -1) {
					std::cerr << std::format("{} is not an active uniform variable\n", name);
				}
				return uniform_locations_[std::string{name}] = location;
			} else {
				return iterator->second;
			}
		}

		const GLuint id_;
		const Shader vertex_shader_, fragment_shader_;
		std::unordered_map<std::string, GLint, string_view_hash, std::equal_to<>> uniform_locations_;
	};
}
