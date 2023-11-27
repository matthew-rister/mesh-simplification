#ifndef SRC_GRAPHICS_DEVICE_H_
#define SRC_GRAPHICS_DEVICE_H_

#include <concepts>

#include <vulkan/vulkan.hpp>

namespace gfx {
struct RankedPhysicalDevice;

class DeviceQueue {
public:
  DeviceQueue(const vk::Device& device,
              const std::uint32_t queue_family_index,
              const std::uint32_t queue_index) noexcept
      : queue_{device.getQueue(queue_family_index, queue_index)}, queue_family_index_{queue_family_index} {}

  [[nodiscard]] const vk::Queue& operator*() const noexcept { return queue_; }
  [[nodiscard]] const vk::Queue* operator->() const noexcept { return &queue_; }

  [[nodiscard]] std::uint32_t queue_family_index() const noexcept { return queue_family_index_; }

private:
  vk::Queue queue_;
  std::uint32_t queue_family_index_;
};

class Device {
public:
  Device(const vk::Instance& instance, const vk::SurfaceKHR& surface);

  [[nodiscard]] const vk::Device& operator*() const noexcept { return *device_; }
  [[nodiscard]] const vk::Device* operator->() const noexcept { return &(*device_); }

  [[nodiscard]] const vk::PhysicalDevice& physical_device() const noexcept { return physical_device_; }

  [[nodiscard]] const DeviceQueue& graphics_queue() const noexcept { return graphics_queue_; }
  [[nodiscard]] const DeviceQueue& present_queue() const noexcept { return present_queue_; }

  void SubmitOneTimeCommandBuffer(std::invocable<const vk::CommandBuffer&> auto&& command_sequence) const {
    const auto command_buffers = device_->allocateCommandBuffersUnique(
        vk::CommandBufferAllocateInfo{.commandPool = *one_time_submit_command_pool_,
                                      .level = vk::CommandBufferLevel::ePrimary,
                                      .commandBufferCount = 1});
    const auto& command_buffer = *command_buffers.front();

    command_buffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    command_sequence(command_buffer);
    command_buffer.end();

    graphics_queue_->submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &command_buffer});
    graphics_queue_->waitIdle();
  }

private:
  explicit Device(RankedPhysicalDevice&& ranked_physical_device);

  vk::PhysicalDevice physical_device_;
  vk::UniqueDevice device_;
  DeviceQueue graphics_queue_, present_queue_;
  vk::UniqueCommandPool one_time_submit_command_pool_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_DEVICE_H_
