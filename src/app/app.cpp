#include "app/app.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "geometry/mesh_simplifier.h"
#include "graphics/obj_loader.h"

namespace {
constexpr auto kWindowWidth = 1920;
constexpr auto kWindowHeight = 1080;

gfx::ArcCamera CreateCamera(const float aspect_ratio) {
  static constexpr glm::vec3 kTarget{0.0f};
  static constexpr glm::vec3 kPosition{0.0f, 0.0f, 2.0f};
  return gfx::ArcCamera{kTarget,
                        kPosition,
                        gfx::ViewFrustum{
                            // NOLINTBEGIN(*-magic-numbers)
                            .field_of_view_y = glm::radians(45.0f),
                            .aspect_ratio = aspect_ratio,
                            .z_near = 0.1f,
                            .z_far = 100'000.0f
                            // NOLINTEND(*-magic-numbers)
                        }};
}

gfx::Mesh CreateMesh(const gfx::Device& device) {
  auto mesh = gfx::obj_loader::LoadMesh(device, "assets/models/bunny.obj");

  // NOLINTBEGIN(*-magic-numbers)
  mesh.Translate(glm::vec3{0.2f, -0.3f, 0.0f});
  mesh.Rotate(glm::vec3{1.0f, 0.0f, 0.0f}, glm::radians(10.0f));
  mesh.Scale(glm::vec3{0.35f});
  // NOLINTEND(*-magic-numbers)

  return mesh;
}

}  // namespace

namespace gfx {

App::App()
    : window_{"Mesh Simplification", kWindowWidth, kWindowHeight},
      engine_{window_},
      camera_{CreateCamera(window_.GetAspectRatio())},
      mesh_{CreateMesh(engine_.device())} {
  window_.OnKeyEvent([this](const auto key, const auto action) { OnKeyEvent(key, action); });
  window_.OnCursorEvent([this](const auto x, const auto y) { OnCursorEvent(x, y); });
  window_.OnScrollEvent([this](const auto y) { OnScrollEvent(y); });
}

void App::Run() {
  while (!window_.IsClosed()) {
    Window::Update();
    engine_.Render(camera_, mesh_);
  }
  engine_.device()->waitIdle();
}

void App::OnKeyEvent(const int key, const int action) {
  if (action != GLFW_PRESS) return;

  switch (key) {
    case GLFW_KEY_ESCAPE:
      window_.Close();
      break;
    case GLFW_KEY_S: {
      static constexpr auto kSimplificationRate = 0.5f;
      mesh_ = mesh::Simplify(engine_.device(), mesh_, kSimplificationRate);
      break;
    }
    default:
      break;
  }
}

void App::OnCursorEvent(const float x, const float y) {
  static std::optional<glm::vec2> maybe_prev_cursor_position;
  const glm::vec2 cursor_position{x, y};

  if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    if (maybe_prev_cursor_position.has_value()) {
      static constexpr auto kRotationSpeed = 0.00390625f;
      const auto delta_cursor_position = cursor_position - *maybe_prev_cursor_position;
      const auto rotation = kRotationSpeed * -delta_cursor_position;
      camera_.Rotate(rotation.x, rotation.y);
    }
    maybe_prev_cursor_position = cursor_position;

  } else if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    if (maybe_prev_cursor_position.has_value()) {
      static constexpr auto kTranslationSpeed = 0.001953125f;
      const auto delta_cursor_position = cursor_position - *maybe_prev_cursor_position;
      const auto translation = kTranslationSpeed * glm::vec2{-delta_cursor_position.x, delta_cursor_position.y};
      camera_.Translate(translation.x, translation.y, 0.0f);
    }
    maybe_prev_cursor_position = cursor_position;

  } else if (maybe_prev_cursor_position.has_value()) {
    maybe_prev_cursor_position = std::nullopt;
  }
}

void App::OnScrollEvent(const float y) {
  static constexpr auto kZoomSpeed = 0.015625f;
  camera_.Zoom(kZoomSpeed * -y);
}

}  // namespace gfx
