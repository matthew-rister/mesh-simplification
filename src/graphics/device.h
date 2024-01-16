#ifndef SRC_GRAPHICS_DEVICE_H_
#define SRC_GRAPHICS_DEVICE_H_

#include <concepts>
#include <cstdint>

#include <vulkan/vulkan.hpp>

namespace gfx {

class PhysicalDevice {
public:
  struct QueueFamilyIndices {
    std::uint32_t graphics_index;
    std::uint32_t present_index;
  };

  PhysicalDevice(const vk::PhysicalDevice physical_device,
                 const vk::PhysicalDeviceLimits& physical_device_limits,
                 const QueueFamilyIndices queue_family_indices)
      : physical_device_{physical_device},
        physical_device_limits_{physical_device_limits},
        queue_family_indices_{queue_family_indices} {}

  [[nodiscard]] vk::PhysicalDevice operator*() const noexcept { return physical_device_; }
  [[nodiscard]] const vk::PhysicalDevice* operator->() const noexcept { return &physical_device_; }

  [[nodiscard]] const vk::PhysicalDeviceLimits& limits() const noexcept { return physical_device_limits_; }
  [[nodiscard]] QueueFamilyIndices queue_family_indices() const noexcept { return queue_family_indices_; }

private:
  vk::PhysicalDevice physical_device_;
  vk::PhysicalDeviceLimits physical_device_limits_;
  QueueFamilyIndices queue_family_indices_;
};

class Device {
public:
  Device(vk::Instance instance, vk::SurfaceKHR surface);

  [[nodiscard]] vk::Device operator*() const noexcept { return *device_; }
  [[nodiscard]] const vk::Device* operator->() const noexcept { return &(*device_); }

  [[nodiscard]] const PhysicalDevice& physical_device() const noexcept { return physical_device_; }

  [[nodiscard]] vk::Queue graphics_queue() const noexcept { return graphics_queue_; }
  [[nodiscard]] vk::Queue present_queue() const noexcept { return present_queue_; }

  void SubmitOneTimeCommandBuffer(std::invocable<const vk::CommandBuffer> auto&& command_sequence) const {
    const auto command_buffers = device_->allocateCommandBuffersUnique(
        vk::CommandBufferAllocateInfo{.commandPool = *one_time_submit_command_pool_,
                                      .level = vk::CommandBufferLevel::ePrimary,
                                      .commandBufferCount = 1});
    const auto command_buffer = *command_buffers.front();

    command_buffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    command_sequence(command_buffer);
    command_buffer.end();

    graphics_queue_.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &command_buffer});
    graphics_queue_.waitIdle();
  }

private:
  PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
  vk::Queue graphics_queue_, present_queue_;
  vk::UniqueCommandPool one_time_submit_command_pool_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_DEVICE_H_
