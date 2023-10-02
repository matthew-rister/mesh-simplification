#include "game.h"

#include <GLFW/glfw3.h>

gfx::Game::Game() noexcept : window_{"VkRender", 1600, 900} {
  window_.OnKeyEvent([this](const auto key, const auto action) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
      window_.Close();
    }
  });
}

void gfx::Game::Run() const {
  while (!window_.IsClosed()) {
    window_.Update();
    engine_.Render(scene_);
  }
}
