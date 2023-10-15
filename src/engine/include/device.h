#pragma once

#include <vulkan/vulkan.hpp>

#include "queue.h"

namespace gfx {

class Device {
public:
  Device(const vk::Instance& instance, const vk::SurfaceKHR& surface);

private:
  struct RankedPhysicalDevice;

  explicit Device(RankedPhysicalDevice&& ranked_physical_device);

  static RankedPhysicalDevice SelectPhysicalDevice(const vk::Instance&, const vk::SurfaceKHR&);
  static RankedPhysicalDevice GetRankedPhysicalDevice(const vk::PhysicalDevice&, const vk::SurfaceKHR&);

  vk::PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
  Queue graphics_queue_, present_queue_;
};

}  // namespace gfx
