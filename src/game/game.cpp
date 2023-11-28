#include "game/game.h"

#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "graphics/arcball.h"
#include "graphics/obj_loader.h"

namespace {

gfx::Camera CreateCamera(const gfx::Window& window) {
  return gfx::Camera{glm::vec3{0.0f, 0.0f, 2.0f},
                     glm::vec3{0.0f},
                     glm::vec3{0.0f, 1.0f, 0.0f},
                     gfx::Camera::ViewFrustum{.field_of_view_y = glm::radians(45.0f),
                                              .aspect_ratio = window.GetAspectRatio(),
                                              .z_near = 0.1f,
                                              .z_far = 10'000.0f}};
}

gfx::Mesh CreateMesh(const gfx::Device& device) {
  const std::filesystem::path mesh_filepath{"assets/models/bunny.obj"};
  auto mesh = gfx::obj_loader::LoadMesh(device, mesh_filepath);
  mesh.Translate(glm::vec3{0.2f, -0.25f, 0.0f});
  mesh.Scale(glm::vec3{0.35f});
  return mesh;
}

}  // namespace

Game::Game()
    : window_{"Mesh Simplification", gfx::Window::Size{.width = 1920, .height = 1080}},
      engine_{window_},
      camera_{CreateCamera(window_)},
      mesh_{CreateMesh(engine_.device())} {
  window_.OnKeyEvent([this](const auto key, const auto action) { HandleKeyEvent(key, action); });
  window_.OnCursorEvent([this](const auto x, const auto y) { HandleCursorEvent(x, y); });
  window_.OnScrollEvent([this](const auto y_offset) { HandleScrollEvent(y_offset); });
}

void Game::Run() {
  while (!window_.IsClosed()) {
    window_.Update();
    engine_.Render(camera_, mesh_);
  }
  engine_.device()->waitIdle();
}

void Game::HandleKeyEvent(const int key, const int action) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    window_.Close();
  }
}

void Game::HandleCursorEvent(const float x, const float y) {
  static constexpr auto kTranslationSpeed = 0.01f;
  static constexpr auto kRotationSpeed = 2.0f;
  static std::optional<glm::vec2> prev_cursor_position;

  if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    const glm::vec2 cursor_position{x, y};
    if (prev_cursor_position.has_value()) {
      const auto window_size = window_.GetSize();
      const auto arcball_rotation = gfx::arcball::GetRotation(*prev_cursor_position, cursor_position, window_size);
      const auto [view_rotation_axis, angle] = arcball_rotation;
      const glm::mat3 model_view_inverse = glm::transpose(camera_.view_transform() * mesh_.transform());
      const auto model_rotation_axis = glm::normalize(model_view_inverse * view_rotation_axis);
      mesh_.Rotate(model_rotation_axis, kRotationSpeed * angle);
    }
    prev_cursor_position = cursor_position;
  } else if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    const glm::vec2 cursor_position{x, y};
    if (prev_cursor_position.has_value()) {
      const auto delta_cursor_position = cursor_position - *prev_cursor_position;
      const glm::vec2 view_translation{delta_cursor_position.x, -delta_cursor_position.y};
      const glm::mat3 model_view_inverse = glm::transpose(camera_.view_transform() * mesh_.transform());
      const auto model_translation = model_view_inverse * glm::vec3{view_translation, 0.0f};
      mesh_.Translate(kTranslationSpeed * model_translation);
    }
    prev_cursor_position = cursor_position;
  } else {
    prev_cursor_position = std::nullopt;
  }
}

void Game::HandleScrollEvent(const float y_offset) {
  static constexpr auto kScaleSpeed = 0.02f;
  mesh_.Scale(glm::vec3{1.0f + kScaleSpeed * y_offset});
}
