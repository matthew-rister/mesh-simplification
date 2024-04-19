#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_PHYSICAL_DEVICE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_PHYSICAL_DEVICE_H_

#include <cstdint>

#include <vulkan/vulkan.hpp>

namespace gfx {

struct QueueFamilyIndices {
  std::uint32_t graphics_index{};
  std::uint32_t present_index{};
};

class PhysicalDevice {
public:
  PhysicalDevice(const vk::Instance instance, const vk::SurfaceKHR surface);

  [[nodiscard]] vk::PhysicalDevice operator*() const noexcept { return physical_device_; }
  [[nodiscard]] const vk::PhysicalDevice* operator->() const noexcept { return &physical_device_; }

  [[nodiscard]] const vk::PhysicalDeviceLimits& limits() const noexcept { return physical_device_limits_; }
  [[nodiscard]] QueueFamilyIndices queue_family_indices() const noexcept { return queue_family_indices_; }

private:
  vk::PhysicalDevice physical_device_;
  vk::PhysicalDeviceLimits physical_device_limits_;
  QueueFamilyIndices queue_family_indices_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_PHYSICAL_DEVICE_H_
