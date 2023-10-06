#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <span>

#include <GLFW/glfw3.h>
#if GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

namespace gfx {

class Window {
public:
  Window(const char* title, int width, int height);

  void OnKeyEvent(std::invocable<int, int> auto&& fn) { on_key_event_ = std::forward<decltype(on_key_event_)>(fn); }

  [[nodiscard]] bool IsClosed() const noexcept { return glfwWindowShouldClose(glfw_window_.get()) == GLFW_TRUE; }
  void Close() const noexcept { glfwSetWindowShouldClose(glfw_window_.get(), GLFW_TRUE); }

  static void Update() noexcept { glfwPollEvents(); }

#ifdef GLFW_INCLUDE_VULKAN
  [[nodiscard]] static std::span<const char* const> GetVulkanInstanceExtensions();

  [[nodiscard]] vk::UniqueSurfaceKHR CreateVulkanSurface(const vk::Instance& instance) const;
#endif

private:
  std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> glfw_window_{nullptr, nullptr};
  std::function<void(int, int)> on_key_event_;
};

}  // namespace gfx
