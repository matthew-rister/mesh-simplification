#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_

#include <vulkan/vulkan.hpp>

#include "graphics/camera.h"
#include "graphics/mesh.h"

namespace gfx {
class Engine;
class Window;

class Scene {
public:
  Scene(const Engine& engine, Window* window);

  [[nodiscard]] const Camera& camera() const { return camera_; }

  void Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const;

private:
  void HandleKeyEvent(const Window& window, const int key, const int action);
  void HandleCursorEvent(const Window& window, const float x, const float y);
  void HandleScrollEvent(const float y_offset);

  Camera camera_;
  Mesh mesh_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
