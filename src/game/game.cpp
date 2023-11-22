#include "game/game.h"

#include <GLFW/glfw3.h>

#include "graphics/obj_loader.h"

namespace {
constexpr auto kWindowHeight = 1600;
constexpr auto kWindowWidth = 900;
}  // namespace

gfx::Game::Game() : window_{"VkRender", kWindowHeight, kWindowWidth}, engine_{window_} {
  window_.OnKeyEvent([this](const auto key, const auto action) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
      window_.Close();
    }
  });

  auto mesh = obj_loader::LoadMesh(engine_.device(), "assets/models/bunny.obj");
  // NOLINTBEGIN(*-magic-numbers)
  mesh.Translate(0.2f, -0.25f, 0.0f);
  mesh.Scale(0.35f, 0.35f, 0.35f);
  // NOLINTEND(*-magic-numbers)
  scene_.AddMesh(std::move(mesh));
}

void gfx::Game::Run() {
  while (!window_.IsClosed()) {
    Window::Update();
    engine_.Render(scene_);
  }
  engine_.device()->waitIdle();
}
