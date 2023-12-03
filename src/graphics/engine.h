#ifndef SRC_GRAPHICS_ENGINE_H_
#define SRC_GRAPHICS_ENGINE_H_

#include <array>
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "graphics/buffer.h"
#include "graphics/device.h"
#include "graphics/image.h"
#include "graphics/instance.h"
#include "graphics/swapchain.h"

namespace gfx {
class Camera;
class Mesh;
class Window;

class Engine {
public:
  explicit Engine(const Window& window);

  [[nodiscard]] const Device& device() const noexcept { return device_; }

  void Render(const Camera& camera, const Mesh& mesh);

private:
  static constexpr std::size_t kMaxRenderFrames = 2;
  std::uint32_t current_frame_index_ = 0;
  Instance instance_;
  vk::UniqueSurfaceKHR surface_;
  Device device_;
  Swapchain swapchain_;
  vk::SampleCountFlagBits msaa_sample_count_;
  Image color_attachment_;
  Image depth_attachment_;
  vk::UniqueRenderPass render_pass_;
  std::vector<vk::UniqueFramebuffer> framebuffers_;
  vk::UniqueDescriptorSetLayout descriptor_set_layout_;
  vk::UniqueDescriptorPool descriptor_pool_;
  std::vector<vk::UniqueDescriptorSet> descriptor_sets_;
  std::vector<Buffer> uniform_buffers_;
  vk::UniquePipelineLayout graphics_pipeline_layout_;
  vk::UniquePipeline graphics_pipeline_;
  vk::UniqueCommandPool command_pool_;
  std::vector<vk::UniqueCommandBuffer> command_buffers_;
  std::array<vk::UniqueSemaphore, kMaxRenderFrames> acquire_next_image_semaphores_;
  std::array<vk::UniqueSemaphore, kMaxRenderFrames> present_image_semaphores_;
  std::array<vk::UniqueFence, kMaxRenderFrames> draw_fences_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_ENGINE_H_
