#include "graphics/texture2d.h"

#include <format>
#include <stdexcept>

#include "stb_image.h"

using namespace qem;
using namespace std;

namespace {

	/** \brief Gets the maximum number of texture units allowed by the host GPU. */
	GLint GetMaxTextureUnits() noexcept {
		static GLint max_texture_units = 0;
		if (!max_texture_units) {
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
		}
		return max_texture_units;
	}
}

Texture2d::Texture2d(const string_view filepath, const int texture_unit_index)
	: texture_unit_index_{texture_unit_index} {

	if (const auto max_texture_units = GetMaxTextureUnits(); texture_unit_index >= max_texture_units) {
		throw out_of_range{
			format("{} exceeds maximum texture unit index {}", texture_unit_index, max_texture_units - 1)
		};
	}

	glActiveTexture(GL_TEXTURE0 + texture_unit_index_);
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	if (int width, height, channels; auto* data = stbi_load(filepath.data(), &width, &height, &channels, 0)) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	} else {
		throw runtime_error{format("Unable to open {}", filepath)};
	}
}
