#pragma once

#include <cstdint>

#include <vulkan/vulkan.hpp>

namespace gfx {

class Queue {
public:
  Queue(const vk::Device& device, const std::uint32_t queue_family_index, const std::uint32_t queue_index) noexcept
      : queue_{device.getQueue(queue_family_index, queue_index)}, queue_family_index_{queue_family_index} {}

private:
  vk::Queue queue_;
  std::uint32_t queue_family_index_;
};

}  // namespace gfx
