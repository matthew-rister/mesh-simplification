#include "window.h"

#include <format>
#include <iostream>
#include <stdexcept>

namespace {

using UniqueGlfwWindow = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>;

class GlfwContext {
public:
  GlfwContext() {
    glfwSetErrorCallback([](const int error_code, const char* description) {
      std::cerr << std::format("GLFW error {}: {}\n", error_code, description);
    });
    if (glfwInit() == GLFW_FALSE) {
      throw std::runtime_error{"GLFW initialization failed"};
    }
  }

  GlfwContext(const GlfwContext&) = delete;
  GlfwContext(GlfwContext&&) noexcept = delete;

  GlfwContext& operator=(const GlfwContext&) = delete;
  GlfwContext& operator=(GlfwContext&&) noexcept = delete;

  ~GlfwContext() noexcept { glfwTerminate(); }
};

}  // namespace

gfx::Window::Window(const char* const title, const int width, const int height) {
  [[maybe_unused]] static const GlfwContext context;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  glfw_window_ = UniqueGlfwWindow{glfwCreateWindow(width, height, title, nullptr, nullptr), glfwDestroyWindow};
  if (glfw_window_ == nullptr) throw std::runtime_error{"GLFW window creation failed"};

  glfwSetWindowUserPointer(glfw_window_.get(), this);
  glfwSetKeyCallback(glfw_window_.get(), [](GLFWwindow* const glfw_window, const int key, const int /*scancode*/,
                                            const int action, const int /*modifiers*/) {
    if (const auto* self = static_cast<const Window*>(glfwGetWindowUserPointer(glfw_window))) {
      self->on_key_event_(key, action);
    }
  });
}
