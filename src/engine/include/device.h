#pragma once

#include <vulkan/vulkan.hpp>

#include "queue.h"

namespace gfx {
struct RankedPhysicalDevice;

class Device {
public:
  Device(const vk::Instance&, const vk::SurfaceKHR&);

  [[nodiscard]] const vk::Device& operator*() const noexcept { return *device_; }
  [[nodiscard]] const vk::Device* operator->() const noexcept { return &(*device_); }

  [[nodiscard]] const vk::PhysicalDevice& physical_device() const noexcept { return physical_device_; }

  [[nodiscard]] const Queue& graphics_queue() const noexcept { return graphics_queue_; }
  [[nodiscard]] const Queue& present_queue() const noexcept { return present_queue_; }

private:
  explicit Device(RankedPhysicalDevice&&);

  vk::PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
  Queue graphics_queue_, present_queue_;
};

}  // namespace gfx
