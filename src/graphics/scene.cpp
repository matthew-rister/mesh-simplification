#include "graphics/scene.h"

#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

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
  window->OnKeyEvent([window](const auto& key, const auto action) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      window->Close();
    }
  });
}

void gfx::Scene::Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const {
  mesh_.Render(command_buffer, pipeline_layout);
}
