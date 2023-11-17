#ifndef SRC_ENGINE_INCLUDE_ENGINE_MEMORY_H_
#define SRC_ENGINE_INCLUDE_ENGINE_MEMORY_H_

#include <memory>

#include <vulkan/vulkan.hpp>

namespace gfx {
class Device;

class Memory {
public:
  Memory(const Device& device,
         const vk::MemoryRequirements& memory_requirements,
         const vk::MemoryPropertyFlags& memory_property_flags);

  Memory(const Memory&) = delete;
  Memory(Memory&& memory) noexcept = default;

  Memory& operator=(const Memory&) = delete;
  Memory& operator=(Memory&&) noexcept = default;

  ~Memory() noexcept { Unmap(); }

  [[nodiscard]] const vk::DeviceMemory& operator*() const noexcept { return *memory_; }
  [[nodiscard]] const vk::DeviceMemory* operator->() const noexcept { return &(*memory_); }

  [[nodiscard]] vk::DeviceSize size() const noexcept { return size_; }

  std::weak_ptr<void> Map();
  void Unmap() noexcept;

private:
  vk::Device device_;
  vk::DeviceSize size_;
  vk::UniqueDeviceMemory memory_;
  std::shared_ptr<void> mapped_memory_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_MEMORY_H_