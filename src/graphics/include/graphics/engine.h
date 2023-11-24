#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_ENGINE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_ENGINE_H_

#include <array>
#include <cstdint>
#include <vector>

#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.hpp>

#include "graphics/device.h"
#include "graphics/image.h"
#include "graphics/instance.h"
#include "graphics/swapchain.h"
#include "graphics/uniform_buffer.h"

namespace gfx {
class Scene;
class Window;

class Engine {
public:
  explicit Engine(const Window& window);

  [[nodiscard]] const Device& device() const noexcept { return device_; }

  void Render(const Scene& scene);

private:
  struct CameraTransforms {
    glm::mat4 view_transform;
    glm::mat4 projection_transform;
  };

  static constexpr std::size_t kMaxRenderFrames = 2;
  std::uint32_t current_frame_index_ = 0;
  Instance instance_;
  vk::UniqueSurfaceKHR surface_;
  Device device_;
  Swapchain swapchain_;
  UniformBuffers<CameraTransforms, kMaxRenderFrames> uniform_buffers_;
  Image depth_buffer_;
  vk::UniqueRenderPass render_pass_;
  std::vector<vk::UniqueFramebuffer> framebuffers_;
  vk::UniquePipelineLayout graphics_pipeline_layout_;
  vk::UniquePipeline graphics_pipeline_;
  vk::UniqueCommandPool command_pool_;
  std::vector<vk::UniqueCommandBuffer> command_buffers_;
  std::array<vk::UniqueSemaphore, kMaxRenderFrames> acquire_next_image_semaphores_;
  std::array<vk::UniqueSemaphore, kMaxRenderFrames> present_image_semaphores_;
  std::array<vk::UniqueFence, kMaxRenderFrames> draw_fences_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_ENGINE_H_
