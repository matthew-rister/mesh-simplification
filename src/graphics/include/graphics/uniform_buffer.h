#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_UNIFORM_BUFFER_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_UNIFORM_BUFFER_H_

#include <algorithm>
#include <ranges>
#include <utility>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "graphics/buffer.h"
#include "graphics/device.h"

namespace gfx {

template <typename T>
class UniformBuffer final : public Buffer<T> {
public:
  UniformBuffer(const Device& device, vk::UniqueDescriptorSet&& descriptor_set)
      : Buffer<T>{device,
                  sizeof(T),
                  vk::BufferUsageFlagBits::eUniformBuffer,
                  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent},
        descriptor_set_{std::move(descriptor_set)} {
    UpdateDescriptorSet(*device, *descriptor_set_, **this);
  }

  [[nodiscard]] const vk::DescriptorSet& descriptor_set() const noexcept { return *descriptor_set_; }

private:
  static void UpdateDescriptorSet(const vk::Device& device,
                                  const vk::DescriptorSet& descriptor_set,
                                  const vk::Buffer& buffer) noexcept {
    const vk::DescriptorBufferInfo buffer_info{.buffer = buffer, .offset = 0, .range = vk::WholeSize};

    device.updateDescriptorSets(vk::WriteDescriptorSet{.dstSet = descriptor_set,
                                                       .dstBinding = 0,
                                                       .dstArrayElement = 0,
                                                       .descriptorCount = 1,
                                                       .descriptorType = vk::DescriptorType::eUniformBuffer,
                                                       .pBufferInfo = &buffer_info},
                                nullptr);
  }

  vk::UniqueDescriptorSet descriptor_set_;
};

template <typename T, std::size_t N>
class UniformBuffers {
public:
  explicit UniformBuffers(const Device& device)
      : descriptor_pool_{CreateDescriptorPool(*device)},
        descriptor_set_layout_{CreateDescriptorSetLayout(*device)},
        uniform_buffers_{CreateUniformBuffers(device, *descriptor_pool_, *descriptor_set_layout_)} {}

  [[nodiscard]] const vk::DescriptorSetLayout& descriptor_set_layout() const noexcept {
    return *descriptor_set_layout_;
  }

  [[nodiscard]] auto&& operator[](this auto&& self, const std::size_t index) noexcept {
    assert(index < self.uniform_buffers_.size());
    return self.uniform_buffers_[index];
  }

private:
  static vk::UniqueDescriptorPool CreateDescriptorPool(const vk::Device& device) {
    constexpr vk::DescriptorPoolSize kDescriptorPoolSize{.type = vk::DescriptorType::eUniformBuffer,
                                                         .descriptorCount = static_cast<std::uint32_t>(N)};

    return device.createDescriptorPoolUnique(
        vk::DescriptorPoolCreateInfo{.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                     .maxSets = static_cast<std::uint32_t>(N),
                                     .poolSizeCount = 1,
                                     .pPoolSizes = &kDescriptorPoolSize});
  }

  static vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(const vk::Device& device) {
    static constexpr vk::DescriptorSetLayoutBinding kDescriptorSetLayoutBinding{
        .binding = 0,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
        .stageFlags = vk::ShaderStageFlagBits::eVertex};

    return device.createDescriptorSetLayoutUnique(
        vk::DescriptorSetLayoutCreateInfo{.bindingCount = 1, .pBindings = &kDescriptorSetLayoutBinding});
  }

  static std::vector<vk::UniqueDescriptorSet> AllocateDescriptorSets(
      const vk::Device& device,
      const vk::DescriptorPool& descriptor_pool,
      const vk::DescriptorSetLayout& descriptor_set_layout) {
    const std::vector descriptor_set_layouts(N, descriptor_set_layout);

    return device.allocateDescriptorSetsUnique(
        vk::DescriptorSetAllocateInfo{.descriptorPool = descriptor_pool,
                                      .descriptorSetCount = static_cast<std::uint32_t>(descriptor_set_layouts.size()),
                                      .pSetLayouts = descriptor_set_layouts.data()});
  }

  static std::vector<UniformBuffer<T>> CreateUniformBuffers(const Device& device,
                                                            const vk::DescriptorPool& descriptor_pool,
                                                            const vk::DescriptorSetLayout& descriptor_set_layout) {
    return AllocateDescriptorSets(*device, descriptor_pool, descriptor_set_layout) | std::views::as_rvalue
           | std::views::transform([&device](vk::UniqueDescriptorSet&& descriptor_set) {
               return UniformBuffer<T>{device, std::move(descriptor_set)};
             })
           | std::ranges::to<std::vector>();
  }

  vk::UniqueDescriptorPool descriptor_pool_;
  vk::UniqueDescriptorSetLayout descriptor_set_layout_;
  std::vector<UniformBuffer<T>> uniform_buffers_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_UNIFORM_BUFFER_H_
