#ifndef GRAPHICS_MEMORY_H_
#define GRAPHICS_MEMORY_H_

#include <utility>

#include <vulkan/vulkan.hpp>

namespace gfx {
class Device;

class Memory {
public:
  Memory(const Device& device,
         const vk::MemoryRequirements& memory_requirements,
         vk::MemoryPropertyFlags memory_property_flags);

  Memory(const Memory&) = delete;
  Memory(Memory&& memory) noexcept { *this = std::move(memory); }

  Memory& operator=(const Memory&) = delete;
  Memory& operator=(Memory&& memory) noexcept;

  ~Memory() noexcept { Unmap(); }

  [[nodiscard]] vk::DeviceMemory operator*() const noexcept { return *memory_; }

  [[nodiscard]] void* Map();
  void Unmap() noexcept;

private:
  vk::Device device_;
  vk::UniqueDeviceMemory memory_;
  void* mapped_memory_ = nullptr;
};

}  // namespace gfx

#endif  // GRAPHICS_MEMORY_H_
