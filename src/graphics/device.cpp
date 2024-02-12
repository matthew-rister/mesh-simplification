#include "graphics/device.h"

#include <array>
#include <cstdint>
#include <ranges>
#include <unordered_set>
#include <vector>

namespace {

vk::UniqueDevice CreateDevice(const gfx::PhysicalDevice& physical_device) {
  static constexpr auto kHighestNormalizedQueuePriority = 1.0f;
  const auto [graphics_index, present_index] = physical_device.queue_family_indices();

  const auto device_queue_create_info =
      std::unordered_set{graphics_index, present_index}  //
      | std::views::transform([](const auto queue_family_index) {
          return vk::DeviceQueueCreateInfo{.queueFamilyIndex = queue_family_index,
                                           .queueCount = 1,
                                           .pQueuePriorities = &kHighestNormalizedQueuePriority};
        })
      | std::ranges::to<std::vector>();

  static constexpr std::array kDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  auto device = physical_device->createDeviceUnique(
      vk::DeviceCreateInfo{.queueCreateInfoCount = static_cast<std::uint32_t>(device_queue_create_info.size()),
                           .pQueueCreateInfos = device_queue_create_info.data(),
                           .enabledExtensionCount = static_cast<std::uint32_t>(kDeviceExtensions.size()),
                           .ppEnabledExtensionNames = kDeviceExtensions.data()});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);
#endif

  return device;
}

}  // namespace

gfx::Device::Device(const vk::Instance instance, const vk::SurfaceKHR surface)
    : physical_device_{instance, surface},
      device_{CreateDevice(physical_device_)},
      graphics_queue_{device_->getQueue(physical_device_.queue_family_indices().graphics_index, 0)},
      present_queue_{device_->getQueue(physical_device_.queue_family_indices().present_index, 0)},
      one_time_submit_command_pool_{device_->createCommandPoolUnique(
          vk::CommandPoolCreateInfo{.flags = vk::CommandPoolCreateFlagBits::eTransient,
                                    .queueFamilyIndex = physical_device_.queue_family_indices().graphics_index})} {}
