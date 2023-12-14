#include "app.h"  // NOLINT(build/include_subdir)

#include <cstdlib>
#include <exception>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "geometry/mesh_simplifier.h"
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

gfx::App::App()
    : window_{"Mesh Simplification", Window::Extent{.width = kWindowWidth, .height = kWindowHeight}},
      engine_{window_},
      camera_{CreateCamera(window_.GetAspectRatio())},
      mesh_{CreateMesh(engine_.device())} {
  window_.OnKeyEvent([this](const auto key, const auto action) { HandleKeyEvent(key, action); });
  window_.OnCursorEvent([this](const auto x, const auto y) { HandleCursorEvent(x, y); });
  window_.OnScrollEvent([this](const auto y_offset) { HandleScrollEvent(y_offset); });
}

void gfx::App::Run() {
  while (!window_.IsClosed()) {
    Window::Update();
    engine_.Render(camera_, mesh_);
  }
  engine_.device()->waitIdle();
}

void gfx::App::HandleKeyEvent(const int key, const int action) {
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

void gfx::App::HandleCursorEvent(const float x, const float y) {
  static constexpr auto kTranslationSpeed = 0.01f;
  static constexpr auto kRotationSpeed = 2.0f;
  static std::optional<glm::vec2> prev_cursor_position;

  if (window_.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    const glm::vec2 cursor_position{x, y};
    if (prev_cursor_position.has_value()) {
      const auto view_rotation = arcball::GetRotation(*prev_cursor_position, cursor_position, window_.GetExtent());
      if (view_rotation.has_value()) {
        const auto& [view_rotation_axis, angle] = *view_rotation;
        const glm::mat3 model_view_inverse = glm::transpose(camera_.view_transform() * mesh_.transform());
        const auto model_rotation_axis = glm::normalize(model_view_inverse * view_rotation_axis);
        mesh_.Rotate(model_rotation_axis, kRotationSpeed * angle);
      }
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

void gfx::App::HandleScrollEvent(const float y) {
  static constexpr auto kScaleSpeed = 0.02f;
  mesh_.Scale(glm::vec3{1.0f + kScaleSpeed * y});
}

int main() {
  try {
    gfx::App app;
    app.Run();
  } catch (const std::system_error& e) {
    std::cerr << '[' << e.code() << "] " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "An unknown error occurred" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
