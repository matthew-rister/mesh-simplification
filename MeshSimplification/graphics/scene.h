#pragma once

#include "graphics/camera.h"
#include "graphics/mesh.h"
#include "graphics/shader_program.h"

namespace qem {
	class Window;

	/** \brief Defines all of the necessary objects (e.g., camera, lighting, mesh, etc.) to render the scene. */
	class Scene {

	public:
		/**
		 * \brief Initializes the scene.
		 * \param window The window to render to.
		 */
		explicit Scene(Window* window);

		/**
		 * \brief Renders the scene.
		 * \param delta_time The amount of time elapsed since the last frame was rendered.
		 */
		void Render(float delta_time);

	private:
		Window* window_;
		Camera camera_;
		Mesh mesh_;
		ShaderProgram shader_program_;
	};
}
