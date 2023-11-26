#include "graphics/scene.h"

#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "graphics/arcball.h"
#include "graphics/engine.h"
#include "graphics/obj_loader.h"
#include "graphics/window.h"

namespace {

gfx::Camera CreateCamera(const gfx::Window& window) {
  return gfx::Camera{glm::vec3{0.0f, 0.0f, 2.0f},
                     glm::vec3{0.0f},
                     glm::vec3{0.0f, 1.0f, 0.0f},
                     gfx::ViewFrustum{.field_of_view_y = glm::radians(45.0f),
                                      .aspect_ratio = window.GetAspectRatio(),
                                      .z_near = 0.1f,
                                      .z_far = 10'000.0f}};
}

gfx::Mesh CreateMesh(const gfx::Device& device) {
  const std::filesystem::path mesh_filepath{"assets/models/bunny.obj"};
  auto mesh = gfx::obj_loader::LoadMesh(device, mesh_filepath);
  mesh.Translate(0.2f, -0.25f, 0.0f);
  mesh.Scale(0.35f, 0.35f, 0.35f);
  return mesh;
}

}  // namespace

gfx::Scene::Scene(const Engine& engine, Window* const window)
    : camera_{CreateCamera(*window)}, mesh_{CreateMesh(engine.device())} {
  window->OnKeyEvent([window, this](const auto key, const auto action) { HandleKeyEvent(*window, key, action); });
  window->OnCursorEvent([window, this](const auto x, const auto y) { HandleCursorEvent(*window, x, y); });
  window->OnScrollEvent([this](const auto y_offset) { HandleScrollEvent(y_offset); });
}

void gfx::Scene::Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const {
  mesh_.Render(command_buffer, pipeline_layout);
}

void gfx::Scene::HandleKeyEvent(const Window& window, const int key, const int action) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    window.Close();
  }
}

void gfx::Scene::HandleCursorEvent(const Window& window, const float x, const float y) {
  static std::optional<glm::vec2> prev_cursor_position;

  if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    const glm::vec2 cursor_position{x, y};
    if (prev_cursor_position.has_value()) {
      static constexpr auto kRotationSpeed = 2.0f;
      const auto arcball_rotation = arcball::GetRotation(*prev_cursor_position, cursor_position, window.GetSize());
      const auto& [view_rotation_axis, angle] = arcball_rotation;
      const glm::mat3 model_view_inverse{glm::transpose(camera_.view_transform() * mesh_.model_transform())};
      const auto model_rotation_axis = glm::normalize(model_view_inverse * view_rotation_axis);
      mesh_.Rotate(model_rotation_axis, kRotationSpeed * angle);
    }
    prev_cursor_position = cursor_position;
  } else if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    const glm::vec2 cursor_position{x, y};
    if (prev_cursor_position.has_value()) {
      static constexpr auto kTranslationSpeed = 0.01f;
      const auto delta_cursor_position = cursor_position - *prev_cursor_position;
      const auto view_translation = glm::vec2{delta_cursor_position.x, -delta_cursor_position.y};
      const glm::mat3 model_view_inverse{glm::transpose(camera_.view_transform() * mesh_.model_transform())};
      const auto model_translation = kTranslationSpeed * model_view_inverse * glm::vec3{view_translation, 0.0f};
      mesh_.Translate(model_translation.x, model_translation.y, model_translation.z);
    }
    prev_cursor_position = cursor_position;
  } else {
    prev_cursor_position = std::nullopt;
  }
}

void gfx::Scene::HandleScrollEvent(const float y_offset) {
  static constexpr auto kScaleSpeed = 0.02f;
  const auto uniform_scale = 1.0f + kScaleSpeed * y_offset;
  mesh_.Scale(uniform_scale, uniform_scale, uniform_scale);
}
