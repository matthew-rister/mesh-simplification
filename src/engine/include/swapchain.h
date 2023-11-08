#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace gfx {
class Device;
class Window;

class Swapchain {
public:
  Swapchain(const Device& device, const Window& window, const vk::SurfaceKHR& surface);

private:
  vk::UniqueSwapchainKHR swapchain_;
  vk::Format image_format_{};
  vk::Extent2D image_extent_;
  std::vector<vk::UniqueImageView> image_views_;
};

}  // namespace gfx
