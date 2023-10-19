#include "game.h"

#include <GLFW/glfw3.h>

namespace {
constexpr std::uint32_t kWindowHeight = 1600;
constexpr std::uint32_t kWindowWidth = 900;
}  // namespace

gfx::Game::Game() noexcept : window_{"VkRender", kWindowHeight, kWindowWidth}, engine_{window_} {
  window_.OnKeyEvent([this](const auto key, const auto action) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
      window_.Close();
    }
  });
}

void gfx::Game::Run() const {
  while (!window_.IsClosed()) {
    Window::Update();
  }
}
