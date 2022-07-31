#include <exception>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "graphics/scene.h"
#include "graphics/window.h"

using namespace qem;
using namespace std;

int main() {

	try {
		constexpr auto kWindowDimensions = make_pair(1280, 960);
		constexpr auto kOpenGlVersion = make_pair(4, 6);
		Window window{"Mesh Simplification", kWindowDimensions, kOpenGlVersion};
		Scene scene{&window};

		for (auto previous_time = static_cast<float>(glfwGetTime()), delta_time = 0.f; !window.IsClosed();) {
			const auto current_time = static_cast<float>(glfwGetTime());
			delta_time = current_time - previous_time;
			previous_time = current_time;
			scene.Render(delta_time);
			window.Update();
		}

	} catch (const exception& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
