#ifndef SRC_GRAPHICS_QUEUE_H_
#define SRC_GRAPHICS_QUEUE_H_

#include <cassert>
#include <cstdint>

#include <vulkan/vulkan.hpp>

namespace gfx {

class QueueFamily {
public:
  QueueFamily(const std::uint32_t index, const std::uint32_t queue_count) noexcept
      : index_{index}, queue_count_{queue_count} {
    assert(queue_count > 0);
  }

  [[nodiscard]] std::uint32_t index() const noexcept { return index_; }
  [[nodiscard]] std::uint32_t queue_count() const noexcept { return queue_count_; }

private:
  std::uint32_t index_;
  std::uint32_t queue_count_;
};

class Queue {
public:
  Queue(const vk::Device& device, const QueueFamily& queue_family, const std::uint32_t queue_index) noexcept
      : queue_family_{queue_family} {
    assert(queue_index < queue_family.queue_count());
    queue_ = device.getQueue(queue_family.index(), queue_index);
  }

  [[nodiscard]] const vk::Queue& operator*() const noexcept { return queue_; }
  [[nodiscard]] const vk::Queue* operator->() const noexcept { return &queue_; }

  [[nodiscard]] const QueueFamily& queue_family() const noexcept { return queue_family_; }

private:
  QueueFamily queue_family_;
  vk::Queue queue_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_QUEUE_H_
