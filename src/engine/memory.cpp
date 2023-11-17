#include "engine/memory.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <ranges>
#include <stdexcept>

#include "engine/device.h"

namespace {

std::uint32_t FindMemoryTypeIndex(const vk::PhysicalDevice& physical_device,
                                  const vk::MemoryRequirements& memory_requirements,
                                  const vk::MemoryPropertyFlags& memory_property_flags) {
  const auto device_memory_properties = physical_device.getMemoryProperties();
  const auto& device_memory_types = device_memory_properties.memoryTypes;

  const auto iterator = std::ranges::find_if(device_memory_types, [&, index = 0u](const auto& memory_type) mutable {
    const auto has_memory_type_bit = memory_requirements.memoryTypeBits & (1u << index++);
    const auto has_memory_flags = (memory_property_flags & memory_type.propertyFlags) == memory_property_flags;
    return has_memory_type_bit && has_memory_flags;
  });

  if (iterator == std::ranges::cend(device_memory_types)) {
    throw std::runtime_error{"Unsupported physical device memory"};
  }

  const auto memory_type_index = std::ranges::distance(std::ranges::cbegin(device_memory_types), iterator);
  return static_cast<std::uint32_t>(memory_type_index);
}

}  // namespace

gfx::Memory::Memory(const Device& device,
                    const vk::MemoryRequirements& memory_requirements,
                    const vk::MemoryPropertyFlags& memory_property_flags)
    : device_{*device},
      size_{memory_requirements.size},
      memory_{device_.allocateMemoryUnique(vk::MemoryAllocateInfo{
          .allocationSize = size_,
          .memoryTypeIndex =
              FindMemoryTypeIndex(device.physical_device(), memory_requirements, memory_property_flags)})} {}

std::weak_ptr<void> gfx::Memory::Map() {
  if (mapped_memory_ == nullptr) {
    mapped_memory_ = std::shared_ptr<void>{device_.mapMemory(*memory_, 0, size_), [](auto* /*data*/) {
                                             // avoid deleting memory managed by vkUnmapMemory
                                           }};
    assert(mapped_memory_ != nullptr);
  }
  return mapped_memory_;
}

void gfx::Memory::Unmap() noexcept {
  if (mapped_memory_ != nullptr) {
    device_.unmapMemory(*memory_);
    mapped_memory_ = nullptr;
  }
}
