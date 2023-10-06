#include "instance.h"

#include <cstdint>
#include <vector>

#include "window.h"

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

gfx::Instance::Instance() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  static const vk::DynamicLoader loader;
  const auto get_instance_proc_address = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(get_instance_proc_address);
#endif

  static constexpr vk::ApplicationInfo kApplicationInfo{.apiVersion = VK_API_VERSION_1_3};
  std::vector<const char*> enabled_layers_names;
#ifndef NDEBUG
  enabled_layers_names.push_back("VK_LAYER_KHRONOS_validation");
#endif
  const auto enabled_extension_names = Window::GetVulkanInstanceExtensions();

  instance_ = vk::createInstanceUnique(
      vk::InstanceCreateInfo{.pApplicationInfo = &kApplicationInfo,
                             .enabledLayerCount = static_cast<std::uint32_t>(enabled_layers_names.size()),
                             .ppEnabledLayerNames = enabled_layers_names.data(),
                             .enabledExtensionCount = static_cast<std::uint32_t>(enabled_extension_names.size()),
                             .ppEnabledExtensionNames = enabled_extension_names.data()});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);
#endif
}
