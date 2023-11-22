#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_BUFFER_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_BUFFER_H_

#include <cassert>
#include <cstring>

#include <vulkan/vulkan.hpp>

#include "graphics/device.h"
#include "graphics/memory.h"

namespace gfx {

template <typename T>
class Buffer {
public:
  Buffer(const Device& device,
         const vk::DeviceSize size,
         const vk::BufferUsageFlags& buffer_usage_flags,
         const vk::MemoryPropertyFlags& memory_property_flags)
      : buffer_{device->createBufferUnique(vk::BufferCreateInfo{.size = size, .usage = buffer_usage_flags})},
        memory_{device, device->getBufferMemoryRequirements(*buffer_), memory_property_flags},
        size_{size} {
    device->bindBufferMemory(*buffer_, *memory_, 0);
  }

  Buffer(const Buffer&) = delete;
  Buffer(Buffer&&) noexcept = default;

  Buffer& operator=(const Buffer&) = delete;
  Buffer& operator=(Buffer&&) noexcept = default;

  virtual ~Buffer() noexcept = default;

  [[nodiscard]] const vk::Buffer& operator*() const noexcept { return *buffer_; }
  [[nodiscard]] const vk::Buffer* operator->() const noexcept { return &(*buffer_); }

  [[nodiscard]] vk::DeviceSize length() const noexcept { return size_ / sizeof(T); }

  void Copy(const vk::ArrayProxy<const T> data) {
    auto* mapped_memory = memory_.Map();
    const auto size_bytes = data.size() * sizeof(T);
    assert(size_bytes <= size_);
    memcpy(mapped_memory, data.data(), size_bytes);
  }

  void Copy(const Device& device, const Buffer& src_buffer) {
    device.SubmitOneTimeCommandBuffer([&src_buffer, this](const auto& command_buffer) {
      command_buffer.copyBuffer(*src_buffer.buffer_, *buffer_, vk::BufferCopy{.size = src_buffer.size_});
    });
  }

private:
  vk::UniqueBuffer buffer_;
  Memory memory_;
  vk::DeviceSize size_;
};

template <typename T>
[[nodiscard]] Buffer<T> CreateDeviceLocalBuffer(const Device& device,
                                                const vk::BufferUsageFlags& buffer_usage_flags,
                                                const vk::ArrayProxy<const T> data) {
  const auto size_bytes = data.size() * sizeof(T);

  Buffer<T> host_visible_buffer{device,
                                size_bytes,
                                vk::BufferUsageFlagBits::eTransferSrc,
                                vk::MemoryPropertyFlagBits::eHostVisible};
  host_visible_buffer.Copy(data);

  Buffer<T> device_local_buffer{device,
                                size_bytes,
                                buffer_usage_flags | vk::BufferUsageFlagBits::eTransferDst,
                                vk::MemoryPropertyFlagBits::eDeviceLocal};
  device_local_buffer.Copy(device, host_visible_buffer);

  return device_local_buffer;
}

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_BUFFER_H_
