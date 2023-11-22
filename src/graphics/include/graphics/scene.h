#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_

#include <utility>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "graphics/mesh.h"

namespace gfx {

class Scene {
public:
  void Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const {
    for (const auto& mesh : meshes_) {
      mesh.Render(command_buffer, pipeline_layout);
    }
  }

  void AddMesh(Mesh&& mesh) { meshes_.push_back(std::move(mesh)); }

private:
  std::vector<Mesh> meshes_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
