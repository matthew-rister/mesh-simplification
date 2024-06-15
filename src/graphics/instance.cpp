#include "graphics/instance.h"

#include <cstdint>
#include <initializer_list>

#include "graphics/window.h"

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

namespace gfx {

Instance::Instance() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  const vk::DynamicLoader dynamic_loader;
  const auto get_instance_proc_addr = dynamic_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(get_instance_proc_addr);
#endif

  static constexpr vk::ApplicationInfo kApplicationInfo{.apiVersion = VK_API_VERSION_1_3};
  static constexpr std::initializer_list<const char* const> kInstanceLayers{
#ifndef NDEBUG
      "VK_LAYER_KHRONOS_validation"
#endif
  };
  const auto instance_extensions = Window::GetInstanceExtensions();

  instance_ = vk::createInstanceUnique(
      vk::InstanceCreateInfo{.pApplicationInfo = &kApplicationInfo,
                             .enabledLayerCount = static_cast<std::uint32_t>(kInstanceLayers.size()),
                             .ppEnabledLayerNames = std::data(kInstanceLayers),
                             .enabledExtensionCount = static_cast<std::uint32_t>(instance_extensions.size()),
                             .ppEnabledExtensionNames = instance_extensions.data()});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);
#endif
}

}  // namespace gfx
