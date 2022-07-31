#pragma once

#include <stdexcept>

#include <glm/vec3.hpp>

namespace qem {

	/**
	 * \brief An enumeration for various material types.
	 * \see Material::FromType
	 */
	enum class MaterialType {
		kBrass,
		kBronze,
		kChrome,
		kCopper,
		kEmerald,
		kGold,
		kJade,
		kObsidian,
		kPearl,
		kRuby,
		kSilver,
		kTurquoise
	};

	/**
	 * \brief A type of material with light reflectance properties used in the Phong reflection model.
	 * \note Material properties courtesy of http://devernay.free.fr/cours/opengl/materials.html
	 */
	struct Material {

		/**
		 * \brief A static factory function to construct a material from a dedicated material type.
		 * \param material_type The material type representing the material to construct.
		 * \return A material containing light reflectance properties for the provided material type.
		 */
		static constexpr Material FromType(const MaterialType material_type) {
			switch (material_type) {
				case MaterialType::kBrass: {
					return Material{
						.ambient = glm::vec3{.329412f, .223529f, .027451f},
						.diffuse = glm::vec3{.780392f, .568627f, .113725f},
						.specular = glm::vec3{.992157f, .941176f, .807843f},
						.shininess = .21794872f,
					};
				}
				case MaterialType::kBronze: {
					return Material{
						.ambient = glm::vec3{.2125f, .1275f, .054f},
						.diffuse = glm::vec3{.714f, .4284f, .18144f},
						.specular = glm::vec3{.393548f, .271906f, .166721f},
						.shininess = .2f
					};
				}
				case MaterialType::kChrome: {
					return Material{
						.ambient = glm::vec3{.25f, .25f, .25f},
						.diffuse = glm::vec3{.4f, .4f, .4f},
						.specular = glm::vec3{.774597f, .774597f, .774597f},
						.shininess = .6f
					};
				}
				case MaterialType::kCopper: {
					return Material{
						.ambient = glm::vec3{.19125f, .0735f, .0225f},
						.diffuse = glm::vec3{.7038f, .27048f, .0828f},
						.specular = glm::vec3{.256777f, .137622f, .086014f},
						.shininess = .6f
					};
				}
				case MaterialType::kEmerald: {
					return Material{
						.ambient = glm::vec3{.0215f, .1745f, .0215f},
						.diffuse = glm::vec3{.07568f, .61424f, .07568f},
						.specular = glm::vec3{.633f, .727811f, .633f},
						.shininess = .6f
					};
				}
				case MaterialType::kGold: {
					return Material{
						.ambient = glm::vec3{.24725f, .1995f, .0745f},
						.diffuse = glm::vec3{.75164f, .60648f, .22648f},
						.specular = glm::vec3{.628281f, .555802f, .366065f},
						.shininess = .4f
					};
				}
				case MaterialType::kJade: {
					return Material{
						.ambient = glm::vec3{.135f, .2225f, .1575f},
						.diffuse = glm::vec3{.54f, .89f, .63f},
						.specular = glm::vec3{.316228f, .316228f, .316228f},
						.shininess = .1f
					};
				}
				case MaterialType::kObsidian: {
					return Material{
						.ambient = glm::vec3{.05375f, .05f, .06625f},
						.diffuse = glm::vec3{.18275f, .17f, .22525f},
						.specular = glm::vec3{.332741f, .328634f, .346435f},
						.shininess = .3f
					};
				}
				case MaterialType::kPearl: {
					return Material{
						.ambient = glm::vec3{.25f, .20725f, .20725f},
						.diffuse = glm::vec3{1.f, .829f, .829f},
						.specular = glm::vec3{.296648f, .296648f, .296648f},
						.shininess = .088f
					};
				}
				case MaterialType::kRuby: {
					return Material{
						.ambient = glm::vec3{.1745f, .01175f, .01175f},
						.diffuse = glm::vec3{.61424f, .04136f, .04136f},
						.specular = glm::vec3{.727811f, .626959f, .626959f},
						.shininess = .6f
					};
				}
				case MaterialType::kSilver: {
					return Material{
						.ambient = glm::vec3{.19225f, .19225f, .19225f},
						.diffuse = glm::vec3{.50754f, .50754f, .50754f},
						.specular = glm::vec3{.508273f, .508273f, .508273f},
						.shininess = .4f
					};
				}
				case MaterialType::kTurquoise: {
					return Material{
						.ambient = glm::vec3{.1f, .18725f, .1745f},
						.diffuse = glm::vec3{.396f, .74151f, .69102f},
						.specular = glm::vec3{.297254f, .30829f, .306678f},
						.shininess = .1f
					};
				}
				default:
					assert(false);
					return Material{};
			}
		}

		/** The ambient light reflected. */
		glm::vec3 ambient;

		/** The diffuse light reflected. */
		glm::vec3 diffuse;

		/** The specular light reflected. */
		glm::vec3 specular;

		/** The degree of shininess reflected from specular highlights. */
		float shininess;
	};
}
