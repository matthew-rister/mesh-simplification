#pragma once

#include "device.h"
#include "instance.h"
#include "shader_module.h"
#include "swapchain.h"

#include <vulkan/vulkan.hpp>

namespace gfx {
class Window;

class Engine {
public:
  explicit Engine(const Window&);

private:
  Instance instance_;
  vk::UniqueSurfaceKHR surface_;
  Device device_;
  Swapchain swapchain_;
  ShaderModule vertex_shader_module_, fragment_shader_module_;
};

}  // namespace gfx
