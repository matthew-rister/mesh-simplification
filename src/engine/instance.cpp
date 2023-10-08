#include "instance.h"

#include <array>
#include <cstdint>

#include "window.h"

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

namespace {

constexpr auto GetInstanceLayers() {
  return std::array{
#ifndef NDEBUG
      "VK_LAYER_KHRONOS_validation",
#endif
      "VK_LAYER_KHRONOS_synchronization2"};
}

}  // namespace

gfx::Instance::Instance() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  static const vk::DynamicLoader kDynamicLoader;
  const auto get_instance_proc_addr = kDynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(get_instance_proc_addr);
#endif

  static constexpr vk::ApplicationInfo kApplicationInfo{.apiVersion = VK_API_VERSION_1_3};
  static constexpr auto kEnabledLayerNames = GetInstanceLayers();
  const auto enabled_extension_names = Window::GetInstanceExtensions();

  instance_ = vk::createInstanceUnique(
      vk::InstanceCreateInfo{.pApplicationInfo = &kApplicationInfo,
                             .enabledLayerCount = static_cast<std::uint32_t>(kEnabledLayerNames.size()),
                             .ppEnabledLayerNames = kEnabledLayerNames.data(),
                             .enabledExtensionCount = static_cast<std::uint32_t>(enabled_extension_names.size()),
                             .ppEnabledExtensionNames = enabled_extension_names.data()});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);
#endif
}
