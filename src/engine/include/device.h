#pragma once

#include <vulkan/vulkan.hpp>

namespace gfx {

class Device {
public:
  Device(const vk::Instance& instance, const vk::SurfaceKHR& surface);

private:
  vk::PhysicalDevice physical_device_;
};

}  // namespace gfx
