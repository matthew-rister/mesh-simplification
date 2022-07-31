#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qem {

	/**
	 * \brief Represents an arc camera that rotates around a central point.
	 * \details The camera uses spherical polar coordinates to specify the position of the camera. However, it deviates
	 *          from standard polar coordinate conventions by initializing the camera to look down the -z-axis when no
	 *          rotations are present resulting in any horizontal or vertical rotations being offset from the +z-axis.
	 */
	class Camera {

	public:
		/**
		 * \brief Initializes a camera.
		 * \param target The central point the camera will rotate around.
		 * \param radius The distance from the camera's origin to the point of rotation.
		 * \param theta The initial horizontal rotation in radians.
		 * \param phi The initial vertical rotation in radians.
		 */
		constexpr Camera(const glm::vec3& target, const float radius, const float theta, const float phi) noexcept
			: target_{target}, radius_{radius}, theta_{theta}, phi_{phi} {}

		/**
		 * \brief Rotates the camera by the specified amount.
		 * \param theta The horizontal rotation in radians.
		 * \param phi The vertical rotation in radians.
		 */
		void Rotate(const float theta, const float phi) {
			theta_ = std::fmod(theta_ + theta, glm::two_pi<float>());
			phi_ = std::clamp(phi_ + phi, kPhiMin, kPhiMax);
		}

		/**
		 * \brief Constructs a view transformation matrix.
		 * \return A matrix representing the camera's frame of reference.
		 */
		[[nodiscard]] glm::mat4 GetViewTransform() const {
			const auto cos_phi = std::cos(phi_);
			const glm::vec3 look_from{
				radius_ * std::sin(theta_) * cos_phi,
				radius_ * std::sin(-phi_),
				radius_ * std::cos(theta_) * cos_phi
			};
			return glm::lookAt(look_from + target_, target_, kWorldUp);
		}

	private:
		static constexpr auto kPhiMin = -glm::half_pi<float>() + std::numeric_limits<float>::epsilon();
		static constexpr auto kPhiMax = glm::half_pi<float>() - std::numeric_limits<float>::epsilon();
		static constexpr glm::vec3 kWorldUp{0.f, 1.f, 0.f};
		glm::vec3 target_;
		float radius_, theta_, phi_;
	};
}
