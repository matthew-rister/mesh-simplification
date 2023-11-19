#ifndef SRC_ENGINE_INCLUDE_ENGINE_WINDOW_H_
#define SRC_ENGINE_INCLUDE_ENGINE_WINDOW_H_

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

  void OnKeyEvent(std::invocable<int, int> auto&& fn) { on_key_event_ = std::forward<decltype(on_key_event_)>(fn); }

  [[nodiscard]] bool IsClosed() const noexcept { return glfwWindowShouldClose(glfw_window_.get()) == GLFW_TRUE; }
  void Close() const noexcept { glfwSetWindowShouldClose(glfw_window_.get(), GLFW_TRUE); }

  [[nodiscard]] std::pair<int, int> GetFramebufferSize() const noexcept;

  static void Update() noexcept { glfwPollEvents(); }

#ifdef GLFW_INCLUDE_VULKAN
  [[nodiscard]] static std::span<const char* const> GetInstanceExtensions();

  [[nodiscard]] vk::UniqueSurfaceKHR CreateSurface(const vk::Instance& instance) const;
#endif

private:
  std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> glfw_window_;
  std::function<void(int, int)> on_key_event_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_WINDOW_H_
