#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_WINDOW_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_WINDOW_H_

#include <concepts>
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

  void OnKeyEvent(std::invocable<int, int> auto&& fn) { key_event_ = std::forward<decltype(key_event_)>(fn); }

  void OnMouseButtonEvent(std::invocable<int, int> auto&& fn) {
    mouse_button_event_ = std::forward<decltype(mouse_button_event_)>(fn);
  }

  [[nodiscard]] bool IsClosed() const noexcept { return glfwWindowShouldClose(glfw_window_.get()) == GLFW_TRUE; }
  void Close() const noexcept { glfwSetWindowShouldClose(glfw_window_.get(), GLFW_TRUE); }

  [[nodiscard]] std::pair<int, int> GetFramebufferSize() const noexcept;
  [[nodiscard]] float GetAspectRatio() const noexcept;

  [[nodiscard]] std::pair<float, float> GetCursorPosition() const noexcept;

  static void Update() noexcept { glfwPollEvents(); }

#ifdef GLFW_INCLUDE_VULKAN
  [[nodiscard]] static std::span<const char* const> GetInstanceExtensions();

  [[nodiscard]] vk::UniqueSurfaceKHR CreateSurface(const vk::Instance& instance) const;
#endif

private:
  std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> glfw_window_;
  std::function<void(int, int)> key_event_;
  std::function<void(int, int)> mouse_button_event_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_WINDOW_H_
