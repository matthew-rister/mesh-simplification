#include "graphics/device.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <unordered_set>
#include <vector>

struct gfx::RankedPhysicalDevice {
  struct QueueFamilies {
    QueueFamily graphics_family;
    QueueFamily present_family;
  };
  std::uint32_t rank{};
  QueueFamilies queue_families;
  vk::PhysicalDevice physical_device;
  vk::PhysicalDeviceLimits physical_device_limits;
};

namespace {

using QueueFamilies = gfx::RankedPhysicalDevice::QueueFamilies;

std::optional<QueueFamilies> FindQueueFamilies(const vk::PhysicalDevice& physical_device,
                                               const vk::SurfaceKHR& surface) {
  std::optional<gfx::QueueFamily> graphics_family;
  std::optional<gfx::QueueFamily> present_family;

  for (std::uint32_t index = 0; const auto& queue_family_properties : physical_device.getQueueFamilyProperties()) {
    if (queue_family_properties.queueFlags & vk::QueueFlagBits::eGraphics) {
      graphics_family = gfx::QueueFamily{index, queue_family_properties.queueCount};
    }
    if (physical_device.getSurfaceSupportKHR(index, surface) == vk::True) {
      present_family = gfx::QueueFamily{index, queue_family_properties.queueCount};
    }
    if (graphics_family.has_value() && present_family.has_value()) {
      return QueueFamilies{.graphics_family = *graphics_family, .present_family = *present_family};
    }
    ++index;
  }

  return std::nullopt;
}

std::optional<gfx::RankedPhysicalDevice> RankPhysicalDevice(const vk::PhysicalDevice& physical_device,
                                                            const vk::SurfaceKHR& surface) {
  return FindQueueFamilies(physical_device, surface).transform([&](const auto& queue_families) {
    const auto physical_device_properties = physical_device.getProperties();
    return gfx::RankedPhysicalDevice{
        .rank = physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu,
        .queue_families = queue_families,
        .physical_device = physical_device,
        .physical_device_limits = physical_device_properties.limits};
  });
}

gfx::RankedPhysicalDevice SelectPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  const auto ranked_physical_devices =
      instance.enumeratePhysicalDevices()
      | std::views::transform([&](const auto& physical_device) { return RankPhysicalDevice(physical_device, surface); })
      | std::views::filter([](const auto& ranked_physical_device) { return ranked_physical_device.has_value(); })
      | std::ranges::to<std::vector>();

  return ranked_physical_devices.empty()
             ? throw std::runtime_error{"Unsupported physical device"}
             : **std::ranges::max_element(ranked_physical_devices,
                                          [](const auto& lhs, const auto& rhs) { return lhs->rank < rhs->rank; });
}

vk::UniqueDevice CreateDevice(const vk::PhysicalDevice& physical_device, const QueueFamilies& queue_families) {
  static constexpr auto kHighestNormalizedQueuePriority = 1.0f;

  const auto device_queue_create_info =
      std::unordered_set{queue_families.graphics_family.index(), queue_families.present_family.index()}
      | std::views::transform([](const auto queue_family_index) {
          return vk::DeviceQueueCreateInfo{.queueFamilyIndex = queue_family_index,
                                           .queueCount = 1,
                                           .pQueuePriorities = &kHighestNormalizedQueuePriority};
        })
      | std::ranges::to<std::vector>();

  constexpr std::array kDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  auto device = physical_device.createDeviceUnique(
      vk::DeviceCreateInfo{.queueCreateInfoCount = static_cast<std::uint32_t>(device_queue_create_info.size()),
                           .pQueueCreateInfos = device_queue_create_info.data(),
                           .enabledExtensionCount = static_cast<std::uint32_t>(kDeviceExtensions.size()),
                           .ppEnabledExtensionNames = kDeviceExtensions.data()});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);
#endif

  return device;
}

vk::UniqueCommandPool CreateOneTimeSubmitCommandPool(const vk::Device& device, const gfx::Queue& graphics_queue) {
  return device.createCommandPoolUnique(
      vk::CommandPoolCreateInfo{.flags = vk::CommandPoolCreateFlagBits::eTransient,
                                .queueFamilyIndex = graphics_queue.queue_family().index()});
}

}  // namespace

gfx::Device::Device(const vk::Instance& instance, const vk::SurfaceKHR& surface)
    : Device{SelectPhysicalDevice(instance, surface)} {}

gfx::Device::Device(const RankedPhysicalDevice& ranked_physical_device)
    : physical_device_{ranked_physical_device.physical_device, ranked_physical_device.physical_device_limits},
      device_{CreateDevice(*physical_device_, ranked_physical_device.queue_families)},
      graphics_queue_{*device_, ranked_physical_device.queue_families.graphics_family, 0},
      present_queue_{*device_, ranked_physical_device.queue_families.present_family, 0},
      one_time_submit_command_pool_{CreateOneTimeSubmitCommandPool(*device_, graphics_queue_)} {}
