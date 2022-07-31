#include "shader_program.h"

#include <fstream>
#include <stdexcept>
#include <vector>

using namespace qem;
using namespace std;

namespace {

	/**
	 * \brief Retrieves the contents of a file.
	 * \param filepath The filepath to load contents from.
	 * \return A string containing the file contents.
	 */
	string Read(const string_view filepath) {

		if (ifstream ifs{filepath.data(), ios::ate | ios::binary}) {
			string source;
			source.reserve(static_cast<std::size_t>(ifs.tellg()));
			ifs.seekg(0, ios::beg);
			source.assign(istreambuf_iterator<char>{ifs}, istreambuf_iterator<char>{});
			return source;
		}

		throw runtime_error{format("Unable to open {}", filepath)};
	}

	/**
	 * \brief Verifies the status of a shader.
	 * \param shader_id The shader ID.
	 * \param status_type The status type to verify.
	 */
	void VerifyShaderStatus(const GLuint shader_id, const GLenum status_type) {
		GLint success;
		glGetShaderiv(shader_id, status_type, &success);

		if (!success) {
			GLsizei log_length;
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
			vector<GLchar> info_log(log_length);
			glGetShaderInfoLog(shader_id, log_length, &log_length, info_log.data());
			throw runtime_error{info_log.data()};
		}
	}

	/**
	 * \brief Verifies the status of a shader program.
	 * \param shader_program_id The shader program ID.
	 * \param status_type The shader program status type to verify.
	 */
	void VerifyShaderProgramStatus(const GLuint shader_program_id, const GLenum status_type) {
		GLint success;
		glGetProgramiv(shader_program_id, status_type, &success);

		if (!success) {
			GLsizei log_length;
			glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &log_length);
			vector<GLchar> info_log(log_length);
			glGetProgramInfoLog(shader_program_id, log_length, &log_length, info_log.data());
			throw runtime_error{info_log.data()};
		}
	}
}

ShaderProgram::Shader::Shader(const GLenum shader_type, const GLchar* const shader_source)
	: id{glCreateShader(shader_type)} {

	if (!id) throw runtime_error{"Shader creation failed"};

	glShaderSource(id, 1, &shader_source, nullptr);
	glCompileShader(id);
	VerifyShaderStatus(id, GL_COMPILE_STATUS);
}

ShaderProgram::ShaderProgram(
	const string_view vertex_shader_filepath, const string_view fragment_shader_filepath)
	: id_{glCreateProgram()},
	  vertex_shader_{GL_VERTEX_SHADER, Read(vertex_shader_filepath).c_str()},
	  fragment_shader_{GL_FRAGMENT_SHADER, Read(fragment_shader_filepath).c_str()} {

	if (!id_) throw runtime_error{"Shader program creation failed"};

	glAttachShader(id_, vertex_shader_.id);
	glAttachShader(id_, fragment_shader_.id);

	glLinkProgram(id_);
	VerifyShaderProgramStatus(id_, GL_LINK_STATUS);

	glValidateProgram(id_);
	VerifyShaderProgramStatus(id_, GL_VALIDATE_STATUS);

	glDetachShader(id_, vertex_shader_.id);
	glDetachShader(id_, fragment_shader_.id);
}
