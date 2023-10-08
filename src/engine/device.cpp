#include "device.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <optional>
#include <ranges>
#include <stdexcept>

namespace {

struct RankedPhysicalDevice {
  vk::PhysicalDevice physical_device;
  std::uint32_t graphics_index, present_index;
  std::int32_t rank;
};

std::optional<std::size_t> SelectGraphicsQueueFamily(const std::vector<vk::QueueFamilyProperties>& queue_families) {
  const auto iterator = std::ranges::find_if(queue_families, [](const auto& queue_family) {
    return static_cast<bool>(queue_family.queueFlags & vk::QueueFlagBits::eGraphics);
  });
  if (iterator != std::ranges::cend(queue_families)) {
    return std::ranges::distance(std::ranges::begin(queue_families), iterator);
  }
  return std::nullopt;
}

std::optional<std::size_t> SelectPresentQueueFamily(const vk::PhysicalDevice& physical_device,
                                                    const vk::SurfaceKHR& surface,
                                                    const std::size_t queue_family_count) {
  const auto queue_family_indices = std::ranges::iota_view(0u, queue_family_count);
  const auto iterator = std::ranges::find_if(queue_family_indices, [&](const auto index) {
    return physical_device.getSurfaceSupportKHR(index, surface) == VK_TRUE;
  });
  if (iterator != std::ranges::cend(queue_family_indices)) {
    return std::ranges::distance(std::ranges::begin(queue_family_indices), iterator);
  }
  return std::nullopt;
}

RankedPhysicalDevice SelectPhysicalDevice(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  static constexpr auto kInvalidPhysicalDeviceRank = std::numeric_limits<std::int32_t>::min();

  const auto ranked_physical_devices =
      instance.enumeratePhysicalDevices() | std::ranges::views::transform([&](const auto& physical_device) {
        const auto queue_family_properties = physical_device.getQueueFamilyProperties();
        const auto graphics_index = SelectGraphicsQueueFamily(queue_family_properties);
        const auto present_index = SelectPresentQueueFamily(physical_device, surface, queue_family_properties.size());

        if (graphics_index.has_value() && present_index.has_value()) {
          return RankedPhysicalDevice{
              .physical_device = physical_device,
              .graphics_index = static_cast<std::uint32_t>(*graphics_index),
              .present_index = static_cast<std::uint32_t>(*present_index),
              .rank = physical_device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu};
        }

        return RankedPhysicalDevice{.rank = kInvalidPhysicalDeviceRank};
      })
      | std::ranges::views::filter(
          [](const auto& ranked_physical_device) { return ranked_physical_device.rank != kInvalidPhysicalDeviceRank; })
      | std::ranges::to<std::vector>();

  return ranked_physical_devices.empty()
             ? throw std::runtime_error{"Invalid physical device"}
             : *std::ranges::max_element(ranked_physical_devices, {}, &RankedPhysicalDevice::rank);
}

}  // namespace

gfx::Device::Device(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
  const auto max_rank_physical_device = SelectPhysicalDevice(instance, surface);
  physical_device_ = max_rank_physical_device.physical_device;
}
