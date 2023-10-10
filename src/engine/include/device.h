#pragma once

#include <vulkan/vulkan.hpp>

namespace gfx {

class Device {
public:
  Device(const vk::Instance& instance, const vk::SurfaceKHR& surface);

private:
  vk::PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
};

}  // namespace gfx
