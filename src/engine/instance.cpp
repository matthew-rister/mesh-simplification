#include "instance.h"

#include <cstdint>
#include <stdexcept>
#include <vector>

#include <GLFW/glfw3.h>

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

gfx::Instance::Instance() {
#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  static const vk::DynamicLoader loader;
  const auto get_instance_proc_address = loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
  VULKAN_HPP_DEFAULT_DISPATCHER.init(get_instance_proc_address);
#endif

  constexpr vk::ApplicationInfo kApplicationInfo{.apiVersion = VK_API_VERSION_1_3};

  std::vector<const char*> required_layer_names;
#ifndef NDEBUG
  required_layer_names.push_back("VK_LAYER_KHRONOS_validation");
#endif

  std::uint32_t required_extension_count{};
  const auto** required_extension_names = glfwGetRequiredInstanceExtensions(&required_extension_count);
  if (required_extension_names == nullptr) {
    throw std::runtime_error{"No Vulkan instance extensions for window surface creation could be found"};
  }

  instance_ = vk::createInstanceUnique(
      vk::InstanceCreateInfo{.pApplicationInfo = &kApplicationInfo,
                             .enabledLayerCount = static_cast<std::uint32_t>(required_layer_names.size()),
                             .ppEnabledLayerNames = required_layer_names.data(),
                             .enabledExtensionCount = required_extension_count,
                             .ppEnabledExtensionNames = required_extension_names});

#if VULKAN_HPP_DISPATCH_LOADER_DYNAMIC == 1
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);
#endif
}
