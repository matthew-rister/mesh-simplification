#ifndef SRC_ENGINE_INCLUDE_ENGINE_BUFFER_H_
#define SRC_ENGINE_INCLUDE_ENGINE_BUFFER_H_

#include <cassert>

#include <vulkan/vulkan.hpp>

#include "engine/device.h"
#include "engine/memory.h"

namespace gfx {

class Buffer {
public:
  Buffer(const Device& device,
         const vk::DeviceSize size,
         const vk::BufferUsageFlags& buffer_usage_flags,
         const vk::MemoryPropertyFlags& memory_property_flags)
      : buffer_{device->createBufferUnique(vk::BufferCreateInfo{.size = size, .usage = buffer_usage_flags})},
        memory_{device, device->getBufferMemoryRequirements(*buffer_), memory_property_flags} {
    device->bindBufferMemory(*buffer_, *memory_, 0);
  }

  [[nodiscard]] const vk::Buffer& operator*() const noexcept { return *buffer_; }
  [[nodiscard]] const vk::Buffer* operator->() const noexcept { return &(*buffer_); }

  template <typename T>
  void Copy(const vk::ArrayProxy<const T> src_data) {
    const auto mapped_memory = memory_.Map();
    assert(!mapped_memory.expired());
    memcpy(mapped_memory.lock().get(), src_data.data(), src_data.size() * sizeof(T));
  }

  void Copy(const Device& device, const Buffer& src_buffer) {
    assert(src_buffer.memory_.size() <= memory_.size());
    device.SubmitOneTimeCommandBuffer([&src_buffer, this](const auto& command_buffer) {
      command_buffer.copyBuffer(*src_buffer.buffer_, *buffer_, vk::BufferCopy{.size = src_buffer.memory_.size()});
    });
  }

private:
  vk::UniqueBuffer buffer_;
  Memory memory_;
};

template <typename T>
[[nodiscard]] Buffer CreateDeviceLocalBuffer(const Device& device,
                                             const vk::BufferUsageFlags& buffer_usage_flags,
                                             const vk::ArrayProxy<const T>& data) {
  const auto size_bytes = data.size() * sizeof(T);

  Buffer host_visible_buffer{device,
                             size_bytes,
                             vk::BufferUsageFlagBits::eTransferSrc,
                             vk::MemoryPropertyFlagBits::eHostVisible};
  host_visible_buffer.Copy(data);

  Buffer device_local_buffer{device,
                             size_bytes,
                             buffer_usage_flags | vk::BufferUsageFlagBits::eTransferDst,
                             vk::MemoryPropertyFlagBits::eDeviceLocal};
  device_local_buffer.Copy(device, host_visible_buffer);

  return device_local_buffer;
}

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_BUFFER_H_
