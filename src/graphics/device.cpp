#include "graphics/device.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <unordered_set>
#include <vector>

struct gfx::RankedPhysicalDevice {
  struct QueueFamilyIndices {
    static constexpr std::uint32_t kInvalidIndex = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t graphics_index = kInvalidIndex;
    std::uint32_t present_index = kInvalidIndex;
  };
  static constexpr std::uint32_t kInvalidRank = 0;
  vk::PhysicalDevice physical_device;
  vk::PhysicalDeviceLimits physical_device_limits;
  QueueFamilyIndices queue_family_indices;
  std::uint32_t rank = kInvalidRank;
};

namespace {

using QueueFamilyIndices = gfx::RankedPhysicalDevice::QueueFamilyIndices;

std::optional<QueueFamilyIndices> FindQueueFamilyIndices(const vk::PhysicalDevice& physical_device,
                                                         const vk::SurfaceKHR& surface) {
  std::optional<std::uint32_t> graphics_index;
  std::optional<std::uint32_t> present_index;

  for (std::uint32_t index = 0; const auto& queue_family_properties : physical_device.getQueueFamilyProperties()) {
    assert(queue_family_properties.queueCount > 0);
    if (static_cast<bool>(queue_family_properties.queueFlags & vk::QueueFlagBits::eGraphics)) {
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

gfx::RankedPhysicalDevice GetRankedPhysicalDevice(const vk::PhysicalDevice& physical_device,
                                                  const vk::SurfaceKHR& surface) {
  return FindQueueFamilyIndices(physical_device, surface)
      .transform([&physical_device](const auto& queue_family_indices) {
        const auto physical_device_properties = physical_device.getProperties();
        return gfx::RankedPhysicalDevice{
            .physical_device = physical_device,
            .physical_device_limits = physical_device_properties.limits,
            .queue_family_indices = queue_family_indices,
            .rank = 1u + (physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)};
      })
      .value_or(gfx::RankedPhysicalDevice{.rank = gfx::RankedPhysicalDevice::kInvalidRank});
}

gfx::RankedPhysicalDevice SelectPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  const auto ranked_physical_devices =
      instance.enumeratePhysicalDevices() | std::views::transform([&surface](auto&& physical_device) {
        return GetRankedPhysicalDevice(physical_device, surface);
      })
      | std::views::filter([](auto&& ranked_physical_device) {
          return ranked_physical_device.rank != gfx::RankedPhysicalDevice::kInvalidRank;
        })
      | std::ranges::to<std::vector>();

  return ranked_physical_devices.empty()
             ? throw std::runtime_error{"Unsupported physical device"}
             : *std::ranges::max_element(ranked_physical_devices, {}, &gfx::RankedPhysicalDevice::rank);
}

vk::UniqueDevice CreateDevice(const vk::PhysicalDevice& physical_device,
                              const QueueFamilyIndices& queue_family_indices) {
  static constexpr auto kHighestNormalizedQueuePriority = 1.0f;

  const auto device_queue_create_info =  // NOLINTNEXTLINE(whitespace/braces)
      std::unordered_set{queue_family_indices.graphics_index, queue_family_indices.present_index}
      | std::views::transform([](const auto queue_family_index) {
          assert(queue_family_index != QueueFamilyIndices::kInvalidIndex);
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
                                .queueFamilyIndex = graphics_queue.queue_family_index()});
}

}  // namespace

gfx::Device::Device(const vk::Instance& instance, const vk::SurfaceKHR& surface)
    : Device{SelectPhysicalDevice(instance, surface)} {}

gfx::Device::Device(RankedPhysicalDevice&& ranked_physical_device)
    : physical_device_{ranked_physical_device.physical_device, ranked_physical_device.physical_device_limits},
      device_{CreateDevice(*physical_device_, ranked_physical_device.queue_family_indices)},
      graphics_queue_{*device_, ranked_physical_device.queue_family_indices.graphics_index, 0},
      present_queue_{*device_, ranked_physical_device.queue_family_indices.present_index, 0},
      one_time_submit_command_pool_{CreateOneTimeSubmitCommandPool(*device_, graphics_queue_)} {}
