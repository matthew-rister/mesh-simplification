#include "game/game.h"

#include <filesystem>

#include <GLFW/glfw3.h>

#include "graphics/camera.h"
#include "graphics/obj_loader.h"

namespace {
constexpr auto kWindowHeight = 1600;
constexpr auto kWindowWidth = 900;

// NOLINTBEGIN(*-magic-numbers)
gfx::Camera CreateCamera(const gfx::Window& window) {
  return gfx::Camera{glm::vec3{0.0f, 0.0f, 2.0f},
                     glm::vec3{0.0f},
                     glm::vec3{0.0f, 1.0f, 0.0f},
                     gfx::ViewFrustum{.field_of_view_y = glm::radians(45.0f),
                                      .aspect_ratio = window.GetAspectRatio(),
                                      .z_near = 0.1f,
                                      .z_far = 10'000.0f}};
}

std::vector<gfx::Mesh> CreateMeshes(const gfx::Device& device) {
  const std::filesystem::path mesh_filepath{"assets/models/bunny.obj"};
  auto mesh = gfx::obj_loader::LoadMesh(device, mesh_filepath);

  mesh.Translate(0.2f, -0.25f, 0.0f);
  mesh.Scale(0.35f, 0.35f, 0.35f);

  std::vector<gfx::Mesh> meshes;
  meshes.emplace_back(std::move(mesh));

  return meshes;
}
// NOLINTEND(*-magic-numbers)

}  // namespace

gfx::Game::Game()
    : window_{"VkRender", kWindowHeight, kWindowWidth},
      engine_{window_},
      scene_{CreateCamera(window_), CreateMeshes(engine_.device())} {
  window_.OnKeyEvent([this](const auto key, const auto action) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
      window_.Close();
    }
  });
}

void gfx::Game::Run() {
  while (!window_.IsClosed()) {
    Window::Update();
    engine_.Render(scene_);
  }
  engine_.device()->waitIdle();
}
