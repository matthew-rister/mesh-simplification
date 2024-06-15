#ifndef GRAPHICS_WINDOW_H_
#define GRAPHICS_WINDOW_H_

#include <functional>
#include <memory>
#include <span>
#include <utility>

#include <GLFW/glfw3.h>
#if GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

namespace gfx {

class Window {
public:
  Window(const char* title, int width, int height);

  [[nodiscard]] std::pair<int, int> GetSize() const noexcept;
  [[nodiscard]] std::pair<int, int> GetFramebufferSize() const noexcept;
  [[nodiscard]] float GetAspectRatio() const noexcept;

  void OnKeyEvent(std::invocable<int, int> auto&& fn) { key_event_handler_ = std::forward<decltype(fn)>(fn); }
  void OnCursorEvent(std::invocable<float, float> auto&& fn) { cursor_event_handler_ = std::forward<decltype(fn)>(fn); }
  void OnScrollEvent(std::invocable<float> auto&& fn) { scroll_event_handler_ = std::forward<decltype(fn)>(fn); }

  [[nodiscard]] bool IsClosed() const noexcept { return glfwWindowShouldClose(window_.get()) == GLFW_TRUE; }
  void Close() const noexcept { glfwSetWindowShouldClose(window_.get(), GLFW_TRUE); }

  [[nodiscard]] bool IsMouseButtonPressed(const int button) const noexcept {
    return glfwGetMouseButton(window_.get(), button) == GLFW_PRESS;
  }

  static void Update() noexcept { glfwPollEvents(); }

#ifdef GLFW_INCLUDE_VULKAN
  [[nodiscard]] static std::span<const char* const> GetInstanceExtensions();
  [[nodiscard]] vk::UniqueSurfaceKHR CreateSurface(vk::Instance instance) const;
#endif

private:
  std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> window_;
  std::function<void(int, int)> key_event_handler_;
  std::function<void(float, float)> cursor_event_handler_;
  std::function<void(float)> scroll_event_handler_;
};

}  // namespace gfx

#endif  // GRAPHICS_WINDOW_H_
