#include "game/game.h"

#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "geometry/half_edge_mesh.h"
#include "graphics/arcball.h"
#include "graphics/obj_loader.h"

namespace {

constexpr auto kWindowWidth = 1920;
constexpr auto kWindowHeight = 1080;

gfx::Camera CreateCamera(const float aspect_ratio) {
  static constexpr glm::vec3 kLookFrom{0.0f, 0.0f, 2.0f};
  static constexpr glm::vec3 kLookAt{0.0f};
  static constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};
  const gfx::Camera::ViewFrustum view_frustum{.field_of_view_y = glm::radians(45.0f),
                                              .aspect_ratio = aspect_ratio,
                                              .z_near = 0.1f,
                                              .z_far = 10'000.0f};
  return gfx::Camera{kLookFrom, kLookAt, kUp, view_frustum};
}

gfx::Mesh CreateMesh(const gfx::Device& device) {
  static constexpr glm::vec3 kTranslation{0.2f, -0.25f, 0.0f};
  static constexpr glm::vec3 kScale{0.35f};
  auto mesh = gfx::obj_loader::LoadMesh(device, "assets/models/bunny.obj");
  mesh.Translate(kTranslation);
  mesh.Scale(kScale);
  return mesh;
}

}  // namespace

gfx::Game::Game()
    : window_{"Mesh Simplification", Window::Size{.width = kWindowWidth, .height = kWindowHeight}},
      engine_{window_},
      camera_{CreateCamera(window_.GetAspectRatio())},
      mesh_{CreateMesh(engine_.device())} {
  window_.OnKeyEvent([this](const auto key, const auto action) { HandleKeyEvent(key, action); });
  window_.OnCursorEvent([this](const auto x, const auto y) { HandleCursorEvent(x, y); });
  window_.OnScrollEvent([this](const auto y_offset) { HandleScrollEvent(y_offset); });
}

void gfx::Game::Run() {
  while (!window_.IsClosed()) {
    Window::Update();
    engine_.Render(camera_, mesh_);
  }
  engine_.device()->waitIdle();
}

void gfx::Game::HandleKeyEvent(const int key, const int action) {
  if (action != GLFW_PRESS) return;

  switch (key) {
    case GLFW_KEY_ESCAPE: {
      window_.Close();
      break;
    }
    case GLFW_KEY_S: {
      const HalfEdgeMesh half_edge_mesh{mesh_};
      mesh_ = half_edge_mesh.ToMesh(engine_.device());
      break;
    }
  }
}

void gfx::Game::HandleCursorEvent(const float x, const float y) {
  static constexpr auto kTranslationSpeed = 0.01f;
  static constexpr auto kRotationSpeed = 2.0f;
  static std::optional<glm::vec2> prev_cursor_position;

  if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    const glm::vec2 cursor_position{x, y};
    if (prev_cursor_position.has_value()) {
      const auto arcball_rotation = arcball::GetRotation(*prev_cursor_position, cursor_position, window_.GetSize());
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

void gfx::Game::HandleScrollEvent(const float y) {
  static constexpr auto kScaleSpeed = 0.02f;
  mesh_.Scale(glm::vec3{1.0f + kScaleSpeed * y});
}
