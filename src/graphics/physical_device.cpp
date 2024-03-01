#include "graphics/physical_device.h"

#include <algorithm>
#include <optional>
#include <ranges>
#include <stdexcept>

namespace {

using QueueFamilyIndices = gfx::PhysicalDevice::QueueFamilyIndices;

struct RankedPhysicalDevice {
  static constexpr auto kInvalidRank = -1;
  vk::PhysicalDevice physical_device;
  vk::PhysicalDeviceLimits physical_device_limits;
  QueueFamilyIndices queue_family_indices;
  int rank{};
};

std::optional<QueueFamilyIndices> FindQueueFamilyIndices(const vk::PhysicalDevice physical_device,
                                                         const vk::SurfaceKHR surface) {
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

RankedPhysicalDevice GetRankedPhysicalDevice(const vk::PhysicalDevice physical_device, const vk::SurfaceKHR surface) {
  return FindQueueFamilyIndices(physical_device, surface)
      .transform([physical_device](const auto queue_family_indices) {
        const auto physical_device_properties = physical_device.getProperties();
        return RankedPhysicalDevice{
            .physical_device = physical_device,
            .physical_device_limits = physical_device_properties.limits,
            .queue_family_indices = queue_family_indices,
            .rank = physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu};
      })
      .value_or(RankedPhysicalDevice{.rank = RankedPhysicalDevice::kInvalidRank});
}

RankedPhysicalDevice SelectPhysicalDevice(const vk::Instance instance, const vk::SurfaceKHR surface) {
  const auto ranked_physical_devices = instance.enumeratePhysicalDevices()
                                       | std::views::transform([surface](const auto physical_device) {
                                           return GetRankedPhysicalDevice(physical_device, surface);
                                         })
                                       | std::views::filter([](const auto& ranked_physical_device) {
                                           return ranked_physical_device.rank != RankedPhysicalDevice::kInvalidRank;
                                         })
                                       | std::ranges::to<std::vector>();

  return ranked_physical_devices.empty()
             ? throw std::runtime_error{"Unsupported physical device"}
             : *std::ranges::max_element(ranked_physical_devices, {}, &RankedPhysicalDevice::rank);
}

}  // namespace

namespace gfx {

PhysicalDevice::PhysicalDevice(const vk::Instance instance, const vk::SurfaceKHR surface) {
  const auto [physical_device, limits, queue_family_indices, _] = SelectPhysicalDevice(instance, surface);
  physical_device_ = physical_device;
  physical_device_limits_ = limits;
  queue_family_indices_ = queue_family_indices;
}

}  // namespace gfx
