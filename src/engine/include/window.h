#pragma once

#include <concepts>
#include <functional>
#include <memory>

#include <GLFW/glfw3.h>

namespace gfx {

class Window {
public:
    Window(const char* title, int width, int height);

    void OnKeyEvent(std::invocable<int, int> auto&& fn) { on_key_event_ = std::forward<decltype(on_key_event_)>(fn); }

    [[nodiscard]] bool Closed() const noexcept { return glfwWindowShouldClose(glfw_window_.get()) == GLFW_TRUE; }
    void Close() const noexcept { glfwSetWindowShouldClose(glfw_window_.get(), GLFW_TRUE); }

    void Update() const noexcept { glfwPollEvents(); }

private:
    std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> glfw_window_{nullptr, nullptr};
    std::function<void(int, int)> on_key_event_;
};

}  // namespace gfx