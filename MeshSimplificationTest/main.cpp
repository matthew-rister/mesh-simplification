#include <gtest/gtest.h>

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace {

	void InitializeGlfw() {
		if (!glfwInit()) throw std::runtime_error{"GLFW initialization failed"};
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	}

	GLFWwindow* CreateGlfwWindow() {
		if (const auto window = glfwCreateWindow(100, 100, "Test", nullptr, nullptr)) {
			glfwMakeContextCurrent(window);
			return window;
		}
		throw std::runtime_error{"Window creation failed"};
	}

	void InitializeGl3w() {
		if (gl3wInit()) {
			throw std::runtime_error{"OpenGL initialization failed"};
		}
	}
}

int main(int argc, char** argv) {
	try {
		InitializeGlfw();
		auto* const window = CreateGlfwWindow();
		InitializeGl3w();
		testing::InitGoogleTest(&argc, argv);
		const auto exit_code = RUN_ALL_TESTS();
		glfwDestroyWindow(window);
		glfwTerminate();
		return exit_code;
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
