#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_

#include <utility>
#include <vector>

#include "graphics/camera.h"
#include "graphics/mesh.h"

namespace gfx {

class Scene {
public:
  Scene(const Camera& camera, std::vector<Mesh>&& meshes) : camera_{camera}, meshes_{std::move(meshes)} {}

  [[nodiscard]] const Camera& camera() const noexcept { return camera_; }

  void Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const {
    for (const auto& mesh : meshes_) {
      mesh.Render(command_buffer, pipeline_layout);
    }
  }

private:
  Camera camera_;
  std::vector<Mesh> meshes_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
