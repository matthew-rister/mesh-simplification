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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);  // TODO(matthew-rister): enable after implementing swapchain recreation
#endif
  }
};

using UniqueGlfwWindow = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>;

UniqueGlfwWindow CreateGlfwWindow(const char* const title, const gfx::Window::Extent extent) {
  [[maybe_unused]] const auto& glfw_context = GlfwContext::Get();
  auto* window = glfwCreateWindow(extent.width, extent.height, title, nullptr, nullptr);
  if (window == nullptr) throw std::runtime_error{"GLFW window creation failed"};
  return UniqueGlfwWindow{window, glfwDestroyWindow};
}

}  // namespace

gfx::Window::Window(const char* const title, const Extent extent) : window_{CreateGlfwWindow(title, extent)} {
  glfwSetWindowUserPointer(window_.get(), this);

  glfwSetKeyCallback(
      window_.get(),
      [](GLFWwindow* const window, const int key, const int /*scancode*/, const int action, const int /*modifiers*/) {
        if (const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window)); self->key_event_handler_) {
          self->key_event_handler_(key, action);
        }
      });

  glfwSetCursorPosCallback(window_.get(), [](GLFWwindow* const window, const double x, const double y) {
    if (const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window)); self->cursor_event_handler_) {
      self->cursor_event_handler_(static_cast<float>(x), static_cast<float>(y));
    }
  });

  glfwSetScrollCallback(window_.get(), [](GLFWwindow* const window, const double /*x_offset*/, const double y_offset) {
    if (const auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window)); self->scroll_event_handler_) {
      self->scroll_event_handler_(static_cast<float>(y_offset));
    }
  });
}

gfx::Window::Extent gfx::Window::GetExtent() const noexcept {
  int width{}, height{};
  glfwGetWindowSize(window_.get(), &width, &height);
  return Extent{.width = width, .height = height};
}

float gfx::Window::GetAspectRatio() const noexcept {
  const auto [width, height] = GetExtent();
  assert(height > 0);
  return static_cast<float>(width) / static_cast<float>(height);
}

gfx::Window::Extent gfx::Window::GetFramebufferExtent() const noexcept {
  int width{}, height{};
  glfwGetFramebufferSize(window_.get(), &width, &height);
  return Extent{.width = width, .height = height};
}

#ifdef GLFW_INCLUDE_VULKAN

std::span<const char* const> gfx::Window::GetInstanceExtensions() {
  std::uint32_t required_extension_count{};
  const auto* const* required_extensions = glfwGetRequiredInstanceExtensions(&required_extension_count);
  if (required_extensions == nullptr) throw std::runtime_error{"No window surface instance extensions"};
  return std::span{required_extensions, required_extension_count};
}

vk::UniqueSurfaceKHR gfx::Window::CreateSurface(const vk::Instance instance) const {
  VkSurfaceKHR surface{};
  const auto result = static_cast<vk::Result>(glfwCreateWindowSurface(instance, window_.get(), nullptr, &surface));
  vk::resultCheck(result, "Window surface creation failed");
  const vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter{instance};
  return vk::UniqueSurfaceKHR{vk::SurfaceKHR{surface}, deleter};
}

#endif
