#include "device.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <unordered_set>

namespace {

struct QueueFamilyIndices {
  static constexpr auto kInvalidIndex = std::numeric_limits<std::uint32_t>::max();
  std::uint32_t graphics_index = kInvalidIndex;
  std::uint32_t present_index = kInvalidIndex;
};

struct RankedPhysicalDevice {
  static constexpr std::int32_t kInvalidRank = -1;
  vk::PhysicalDevice physical_device;
  QueueFamilyIndices queue_family_indices;
  std::int32_t rank = kInvalidRank;
};

std::optional<std::uint32_t> FindGraphicsQueueFamily(const std::vector<vk::QueueFamilyProperties>& queue_families) {
  const auto iterator = std::ranges::find_if(queue_families, [](const auto& queue_family) noexcept {
    return static_cast<bool>(queue_family.queueFlags & vk::QueueFlagBits::eGraphics);
  });
  if (iterator != std::ranges::cend(queue_families)) {
    const auto index = std::ranges::distance(std::ranges::begin(queue_families), iterator);
    return static_cast<std::uint32_t>(index);
  }
  return std::nullopt;
}

std::optional<std::uint32_t> FindPresentQueueFamily(const vk::PhysicalDevice& physical_device,
                                                    const vk::SurfaceKHR& surface,
                                                    const std::size_t queue_family_count) {
  const auto queue_family_indices = std::ranges::views::iota(0u, queue_family_count);
  const auto iterator = std::ranges::find_if(queue_family_indices, [&](const auto index) {
    return physical_device.getSurfaceSupportKHR(index, surface) == VK_TRUE;
  });
  if (iterator != std::ranges::cend(queue_family_indices)) {
    const auto index = std::ranges::distance(std::ranges::begin(queue_family_indices), iterator);
    return static_cast<std::uint32_t>(index);
  }
  return std::nullopt;
}

RankedPhysicalDevice GetRankedPhysicalDevice(const vk::PhysicalDevice& physical_device, const vk::SurfaceKHR& surface) {
  const auto queue_families = physical_device.getQueueFamilyProperties();
  if (const auto graphics_index = FindGraphicsQueueFamily(queue_families)) {
    if (const auto present_index = FindPresentQueueFamily(physical_device, surface, queue_families.size())) {
      return RankedPhysicalDevice{
          .physical_device = physical_device,
          .queue_family_indices = QueueFamilyIndices{*graphics_index, *present_index},
          .rank = physical_device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu};
    }
  }
  return RankedPhysicalDevice{.rank = RankedPhysicalDevice::kInvalidRank};
}

RankedPhysicalDevice SelectPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  const auto ranked_physical_devices = instance.enumeratePhysicalDevices()
                                       | std::ranges::views::transform([&](const auto& physical_device) {
                                           return GetRankedPhysicalDevice(physical_device, surface);
                                         })
                                       | std::ranges::views::filter([](const auto& ranked_physical_device) noexcept {
                                           return ranked_physical_device.rank != RankedPhysicalDevice::kInvalidRank;
                                         })
                                       | std::ranges::to<std::vector>();

  return ranked_physical_devices.empty()
             ? throw std::runtime_error{"Invalid physical device"}
             : *std::ranges::max_element(ranked_physical_devices, {}, &RankedPhysicalDevice::rank);
}

std::vector<vk::DeviceQueueCreateInfo> GetDeviceQueueCreateInfo(const QueueFamilyIndices& queue_family_indices) {
  static constexpr auto kHighestNormalizedQueuePriority = 1.0f;

  return std::unordered_set{queue_family_indices.graphics_index, queue_family_indices.present_index}
         | std::ranges::views::transform([](const auto queue_family_index) noexcept {
             assert(queue_family_index != QueueFamilyIndices::kInvalidIndex);
             return vk::DeviceQueueCreateInfo{.queueFamilyIndex = queue_family_index,
                                              .queueCount = 1,
                                              .pQueuePriorities = &kHighestNormalizedQueuePriority};
           })
         | std::ranges::to<std::vector>();
}

vk::UniqueDevice CreateDevice(const vk::PhysicalDevice& physical_device,
                              const QueueFamilyIndices& queue_family_indices) {
  const auto device_queue_create_info = GetDeviceQueueCreateInfo(queue_family_indices);
  static constexpr std::array kRequiredDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  auto device = physical_device.createDeviceUnique(
      vk::DeviceCreateInfo{.queueCreateInfoCount = static_cast<std::uint32_t>(device_queue_create_info.size()),
                           .pQueueCreateInfos = device_queue_create_info.data(),
                           .enabledExtensionCount = static_cast<std::uint32_t>(kRequiredDeviceExtensions.size()),
                           .ppEnabledExtensionNames = kRequiredDeviceExtensions.data()});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);
#endif

  return device;
}

}  // namespace

gfx::Device::Device(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  const auto [physical_device, queue_family_indices, _] = SelectPhysicalDevice(instance, surface);
  physical_device_ = physical_device;
  device_ = CreateDevice(physical_device_, queue_family_indices);
}
