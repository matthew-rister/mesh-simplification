#pragma once

#include <string_view>

#include <GL/gl3w.h>

namespace qem {

	/** \brief A 2D texture in OpenGL. */
	class Texture2d {

	public:
		/**
		 * \brief Initializes a 2D texture.
		 * \param filepath The filepath of the texture to load.
		 * \param texture_unit_index The index to bind the texture to.
		 * \throw std::out_of_range Indicates \p index exceeds the maximum number of allowed texture units.
		 * \throw std::runtime_error Indicates the file cannot be opened.
		 */
		explicit Texture2d(std::string_view filepath, int texture_unit_index = 0);

		~Texture2d() { glDeleteTextures(1, &id_); }

		Texture2d(const Texture2d&) = delete;
		Texture2d& operator=(const Texture2d&) = delete;

		Texture2d(Texture2d&&) noexcept = delete;
		Texture2d& operator=(Texture2d&&) noexcept = delete;

		/** \brief Binds this texture for immediate use in rendering. */
		void Bind() const noexcept {
			glActiveTexture(GL_TEXTURE0 + texture_unit_index_);
			glBindTexture(GL_TEXTURE_2D, id_);
		}

	private:
		GLuint id_ = 0;
		int texture_unit_index_;
	};
}
