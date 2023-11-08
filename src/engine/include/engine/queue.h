#ifndef SRC_ENGINE_INCLUDE_ENGINE_QUEUE_H_
#define SRC_ENGINE_INCLUDE_ENGINE_QUEUE_H_

#include <cstdint>

#include <vulkan/vulkan.hpp>

namespace gfx {

class Queue {
public:
  Queue(const vk::Device& device, const std::uint32_t queue_family_index, const std::uint32_t queue_index) noexcept
      : queue_{device.getQueue(queue_family_index, queue_index)}, queue_family_index_{queue_family_index} {}

  [[nodiscard]] std::uint32_t queue_family_index() const noexcept { return queue_family_index_; }

private:
  vk::Queue queue_;
  std::uint32_t queue_family_index_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_QUEUE_H_
