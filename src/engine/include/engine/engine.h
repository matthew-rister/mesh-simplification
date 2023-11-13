#ifndef SRC_ENGINE_INCLUDE_ENGINE_ENGINE_H_
#define SRC_ENGINE_INCLUDE_ENGINE_ENGINE_H_

#include <vector>

#include <vulkan/vulkan.hpp>

#include "engine/device.h"
#include "engine/instance.h"
#include "engine/swapchain.h"

namespace gfx {
class Window;

class Engine {
public:
  explicit Engine(const Window& window);

  Engine(const Engine&) = delete;
  Engine(Engine&&) noexcept = delete;

  Engine& operator=(const Engine&) = delete;
  Engine& operator=(Engine&&) noexcept = delete;

  ~Engine() noexcept;

  void Render();

private:
  Instance instance_;
  vk::UniqueSurfaceKHR surface_;
  Device device_;
  Swapchain swapchain_;
  vk::UniqueRenderPass render_pass_;
  std::vector<vk::UniqueFramebuffer> framebuffers_;
  vk::UniquePipelineLayout graphics_pipeline_layout_;
  vk::UniquePipeline graphics_pipeline_;
  vk::UniqueCommandPool command_pool_;
  std::vector<vk::UniqueCommandBuffer> command_buffers_;
  std::vector<vk::UniqueSemaphore> acquire_next_image_semaphores_, present_image_semaphores_;
  std::vector<vk::UniqueFence> draw_fences_;
  std::uint32_t current_frame_index_ = 0;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_ENGINE_H_
