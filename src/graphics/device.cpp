#include "graphics/device.h"

#include <algorithm>
#include <array>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace {

struct RankedPhysicalDevice {
  int rank{};
  gfx::PhysicalDevice physical_device;
};

using QueueFamilyIndices = gfx::PhysicalDevice::QueueFamilyIndices;

std::optional<QueueFamilyIndices> FindQueueFamilyIndices(const vk::PhysicalDevice& physical_device,
                                                         const vk::SurfaceKHR& surface) {
  std::optional<std::uint32_t> graphics_index, present_index;
  for (std::uint32_t index = 0; const auto& queue_family_properties : physical_device.getQueueFamilyProperties()) {
    if (queue_family_properties.queueFlags & vk::QueueFlagBits::eGraphics) {
      graphics_index = index;
    }
    if (physical_device.getSurfaceSupportKHR(index, surface) == vk::True) {
      present_index = index;
    }
    if (graphics_index.has_value() && present_index.has_value()) {
      return QueueFamilyIndices{.graphics_index = *graphics_index, .present_index = *present_index};
    }
    ++index;
  }
  return std::nullopt;
}

std::optional<RankedPhysicalDevice> RankPhysicalDevice(const vk::PhysicalDevice& physical_device,
                                                       const vk::SurfaceKHR& surface) {
  return FindQueueFamilyIndices(physical_device, surface).transform([&](const auto& queue_family_indices) {
    const auto physical_device_properties = physical_device.getProperties();
    return RankedPhysicalDevice{
        .rank = physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu,
        .physical_device =
            gfx::PhysicalDevice{physical_device, physical_device_properties.limits, queue_family_indices}};
  });
}

gfx::PhysicalDevice SelectPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  std::optional<RankedPhysicalDevice> max_rank_physical_device;
  for (const auto& physical_device : instance.enumeratePhysicalDevices()) {
    if (const auto ranked_physical_device = RankPhysicalDevice(physical_device, surface)) {
      if (!max_rank_physical_device.has_value() || ranked_physical_device->rank > max_rank_physical_device->rank) {
        max_rank_physical_device = ranked_physical_device;
      }
    }
  }
  return max_rank_physical_device.has_value() ? max_rank_physical_device->physical_device
                                              : throw std::runtime_error{"Unsupported physical device"};
}

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

gfx::Device::Device(const vk::Instance& instance, const vk::SurfaceKHR& surface)
    : physical_device_{SelectPhysicalDevice(instance, surface)},
      device_{CreateDevice(physical_device_)},
      graphics_queue_{device_->getQueue(physical_device_.queue_family_indices().graphics_index, 0)},
      present_queue_{device_->getQueue(physical_device_.queue_family_indices().present_index, 0)},
      one_time_submit_command_pool_{device_->createCommandPoolUnique(
          vk::CommandPoolCreateInfo{.flags = vk::CommandPoolCreateFlagBits::eTransient,
                                    .queueFamilyIndex = physical_device_.queue_family_indices().graphics_index})} {}
