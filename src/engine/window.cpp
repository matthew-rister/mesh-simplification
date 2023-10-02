#include "window.h"

#include <format>
#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>

namespace {

class GlfwContext {
public:
  static const GlfwContext& Initialize() {
    static const GlfwContext instance;
    return instance;
  }

  GlfwContext(const GlfwContext&) = delete;
  GlfwContext(GlfwContext&&) noexcept = delete;

  GlfwContext& operator=(const GlfwContext&) = delete;
  GlfwContext& operator=(GlfwContext&&) noexcept = delete;

  ~GlfwContext() noexcept { glfwTerminate(); }

private:
  GlfwContext() {
    glfwSetErrorCallback([](const int error_code, const char* description) {
      std::cerr << std::format("GLFW error {}: {}\n", error_code, description);
    });
    if (glfwInit() == GLFW_FALSE) {
      throw std::runtime_error{"GLFW initialization failed"};
    }
    if (glfwVulkanSupported() == GLFW_FALSE) {
      throw std::runtime_error{"No Vulkan loader or installable client driver could be found"};
    }
  }
};

const auto& glfw_context = GlfwContext::Initialize();

using UniqueGlfwWindow = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>;

UniqueGlfwWindow CreateGlfwWindow(const char* const title, const int width, const int height) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  auto* glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (glfw_window == nullptr) throw std::runtime_error{"GLFW window creation failed"};

  return UniqueGlfwWindow{glfw_window, glfwDestroyWindow};
}

}  // namespace

gfx::Window::Window(const char* const title, const int width, const int height)
    : glfw_window_{CreateGlfwWindow(title, width, height)} {
  glfwSetWindowUserPointer(glfw_window_.get(), this);

  glfwSetKeyCallback(glfw_window_.get(), [](GLFWwindow* const glfw_window, const int key, const int /*scancode*/,
                                            const int action, const int /*modifiers*/) {
    if (const auto* self = static_cast<const Window*>(glfwGetWindowUserPointer(glfw_window))) {
      self->on_key_event_(key, action);
    }
  });
}
