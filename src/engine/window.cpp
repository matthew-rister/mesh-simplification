#include "window.h"

#include <format>
#include <iostream>
#include <stdexcept>

namespace {

using UniqueGlfwWindow = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>;

class GlfwContext {
public:
  GlfwContext() {
    glfwSetErrorCallback([](const int error_code, const char* const description) {
      std::cerr << std::format("GLFW error {}: {}\n", error_code, description);
    });
    if (glfwInit() == GLFW_FALSE) {
      throw std::runtime_error{"GLFW initialization failed"};
    }
#ifdef GLFW_INCLUDE_VULKAN
    if (glfwVulkanSupported() == GLFW_FALSE) {
      throw std::runtime_error{"No Vulkan loader or installable client driver could be found"};
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
  }

  GlfwContext(const GlfwContext&) = delete;
  GlfwContext(GlfwContext&&) noexcept = delete;

  GlfwContext& operator=(const GlfwContext&) = delete;
  GlfwContext& operator=(GlfwContext&&) noexcept = delete;

  ~GlfwContext() noexcept { glfwTerminate(); }
};

}  // namespace

gfx::Window::Window(const char* const title, const int width, const int height) {
  [[maybe_unused]] static const GlfwContext kGlfwContext;

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

#ifdef GLFW_INCLUDE_VULKAN

std::span<const char* const> gfx::Window::GetVulkanInstanceExtensions() {
  std::uint32_t required_extension_count{};
  const auto* const* required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);
  if (required_extensions == nullptr) {
    throw std::runtime_error{"No Vulkan instance extensions for window surface creation could be found"};
  }
  return std::span{required_extensions, required_extension_count};  // pointer lifetime managed by GLFW
}

vk::UniqueSurfaceKHR gfx::Window::CreateVulkanSurface(const vk::Instance& instance) const {
  VkSurfaceKHR surface{};
  const auto result = glfwCreateWindowSurface(instance, glfw_window_.get(), nullptr, &surface);
  vk::resultCheck(vk::Result{result}, "Vulkan surface creation failed");
  const vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter{instance};
  return vk::UniqueSurfaceKHR{vk::SurfaceKHR{surface}, deleter};
}

#endif
