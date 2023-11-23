#include "graphics/window.h"

#include <cassert>
#include <format>
#include <iostream>
#include <print>
#include <stdexcept>

namespace {

class GlfwContext {
public:
  static const GlfwContext& Get() {
    static const GlfwContext kInstance;
    return kInstance;
  }

  GlfwContext(const GlfwContext&) = delete;
  GlfwContext(GlfwContext&&) noexcept = delete;

  GlfwContext& operator=(const GlfwContext&) = delete;
  GlfwContext& operator=(GlfwContext&&) noexcept = delete;

  ~GlfwContext() noexcept { glfwTerminate(); }

private:
  GlfwContext() {
#ifndef NDEBUG
    glfwSetErrorCallback([](const int error_code, const char* const description) {
      std::println(std::cerr, "GLFW error {}: {}", error_code, description);
    });
#endif
    if (glfwInit() == GLFW_FALSE) {
      throw std::runtime_error{"GLFW initialization failed"};
    }
#ifdef GLFW_INCLUDE_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
  }
};

using UniqueGlfwWindow = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>;

UniqueGlfwWindow CreateGlfwWindow(const char* const title, const int width, const int height) {
  [[maybe_unused]] const auto& glfw_context = GlfwContext::Get();
  auto* glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (glfw_window == nullptr) throw std::runtime_error{"GLFW window creation failed"};
  return UniqueGlfwWindow{glfw_window, glfwDestroyWindow};
}

}  // namespace

gfx::Window::Window(const char* const title, const int width, const int height)
    : glfw_window_{CreateGlfwWindow(title, width, height)} {
  glfwSetWindowUserPointer(glfw_window_.get(), this);

  glfwSetKeyCallback(
      glfw_window_.get(),
      [](GLFWwindow* glfw_window, const int key, const int /*scancode*/, const int action, const int /*modifiers*/) {
        const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window));
        assert(self != nullptr);
        if (self->on_key_event_) self->on_key_event_(key, action);
      });
}

std::pair<int, int> gfx::Window::GetFramebufferSize() const noexcept {
  int width{}, height{};
  glfwGetFramebufferSize(glfw_window_.get(), &width, &height);
  return std::pair{width, height};
}
float gfx::Window::GetAspectRatio() const noexcept {
  int width{}, height{};
  glfwGetWindowSize(glfw_window_.get(), &width, &height);  // TODO(matthew-rister): can this be getFramebufferSize?
  return static_cast<float>(width) / static_cast<float>(height);
}

#ifdef GLFW_INCLUDE_VULKAN

std::span<const char* const> gfx::Window::GetInstanceExtensions() {
  std::uint32_t required_extension_count{};
  const auto* const* required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);
  if (required_extensions == nullptr) throw std::runtime_error{"No window surface instance extensions"};
  return std::span{required_extensions, required_extension_count};  // pointer lifetime managed by GLFW
}

vk::UniqueSurfaceKHR gfx::Window::CreateSurface(const vk::Instance& instance) const {
  VkSurfaceKHR surface{};
  const auto result = static_cast<vk::Result>(glfwCreateWindowSurface(instance, glfw_window_.get(), nullptr, &surface));
  vk::resultCheck(result, std::format("Window surface creation failed with error {}", vk::to_string(result)).c_str());
  const vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter{instance};
  return vk::UniqueSurfaceKHR{vk::SurfaceKHR{surface}, deleter};
}

#endif
