#include "instance.h"

#include <cstdint>
#include <vector>

#include "window.h"

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

gfx::Instance::Instance() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  static const vk::DynamicLoader kDynamicLoader;
  const auto get_instance_proc_addr = kDynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(get_instance_proc_addr);
#endif

  static constexpr vk::ApplicationInfo kApplicationInfo{.apiVersion = VK_API_VERSION_1_3};
  const std::vector<const char*> instance_layers{
#ifndef NDEBUG
      "VK_LAYER_KHRONOS_validation"
#endif
  };
  const auto instance_extensions = Window::GetInstanceExtensions();

  instance_ = vk::createInstanceUnique(
      vk::InstanceCreateInfo{.pApplicationInfo = &kApplicationInfo,
                             .enabledLayerCount = static_cast<std::uint32_t>(instance_layers.size()),
                             .ppEnabledLayerNames = instance_layers.data(),
                             .enabledExtensionCount = static_cast<std::uint32_t>(instance_extensions.size()),
                             .ppEnabledExtensionNames = instance_extensions.data()});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);
#endif
}
