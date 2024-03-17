#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_WINDOW_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_WINDOW_H_

#include <functional>
#include <memory>
#include <span>
#include <utility>

#include <GLFW/glfw3.h>
#if GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.hpp>
#endif

namespace gfx {

/** \brief A window to display rendered output to. */
class Window {
public:
  /**
   * \brief Initializes a window.
   * \param title The window title.
   * \param width The window width.
   * \param height The window height.
   */
  Window(const char* title, int width, int height);

  /** \brief Gets the window width and height in screen coordinates. */
  [[nodiscard]] std::pair<int, int> GetSize() const noexcept;

  /** \brief Gets the framebuffer width and height in pixels. */
  [[nodiscard]] std::pair<int, int> GetFramebufferSize() const noexcept;

  /** \brief Gets the ratio of the window's width by height. */
  [[nodiscard]] float GetAspectRatio() const noexcept;

  /**
   * \brief Sets an event handler to be invoked when a key event is registered.
   * \param fn The event handler parameterized by the registered GLFW key code and action.
   */
  void OnKeyEvent(std::invocable<int, int> auto&& fn) { key_event_handler_ = std::forward<decltype(fn)>(fn); }

  /**
   * \brief Sets an event handler to be invoked when a cursor event is registered.
   * \param fn The event handler parameterized by the registered (x,y) cursor position in screen coordinates.
   */
  void OnCursorEvent(std::invocable<float, float> auto&& fn) { cursor_event_handler_ = std::forward<decltype(fn)>(fn); }

  /**
   * \brief Sets an event handler to be invoked when a scroll event is registered.
   * \param fn The event handler parameterized by the registered vertical scroll offset.
   */
  void OnScrollEvent(std::invocable<float> auto&& fn) { scroll_event_handler_ = std::forward<decltype(fn)>(fn); }

  /**
   * \brief Determines if the window is closed.
   * \return \c true if the window is closed, otherwise \c false.
   */
  [[nodiscard]] bool IsClosed() const noexcept { return glfwWindowShouldClose(window_.get()) == GLFW_TRUE; }

  /** \brief Closes the window. */
  void Close() const noexcept { glfwSetWindowShouldClose(window_.get(), GLFW_TRUE); }

  /**
   * \brief Determines if a mouse button is pressed.
   * \param button The GLFW mouse button to evaluate (e.g., GLFW_MOUSE_BUTTON_LEFT).
   * \return \c true if \p button is pressed, otherwise \c false.
   */
  [[nodiscard]] bool IsMouseButtonPressed(const int button) const noexcept {
    return glfwGetMouseButton(window_.get(), button) == GLFW_PRESS;
  }

  /** \brief Processes event handlers for registered input events. */
  static void Update() noexcept { glfwPollEvents(); }

#ifdef GLFW_INCLUDE_VULKAN
  /**
   * \brief Gets instance extensions required to create a Vulkan surface.
   * \return The required Vulkan instance extension names.
   * \remark String pointer lifetimes are managed by GLFW.
   */
  [[nodiscard]] static std::span<const char* const> GetInstanceExtensions();

  /**
   * \brief Creates a Vulkan surface.
   * \param instance The Vulkan instance used to create the surface.
   * \return A unique handle to the created Vulkan surface.
   */
  [[nodiscard]] vk::UniqueSurfaceKHR CreateSurface(vk::Instance instance) const;
#endif

private:
  std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> window_;
  std::function<void(int, int)> key_event_handler_;
  std::function<void(float, float)> cursor_event_handler_;
  std::function<void(float)> scroll_event_handler_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_WINDOW_H_
