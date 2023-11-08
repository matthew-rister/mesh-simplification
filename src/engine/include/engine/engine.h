#ifndef SRC_ENGINE_INCLUDE_ENGINE_ENGINE_H_
#define SRC_ENGINE_INCLUDE_ENGINE_ENGINE_H_

#include <vulkan/vulkan.hpp>

#include "engine/device.h"
#include "engine/instance.h"
#include "engine/shader_module.h"
#include "engine/swapchain.h"

namespace gfx {
class Window;

class Engine {
public:
  explicit Engine(const Window& window);

private:
  Instance instance_;
  vk::UniqueSurfaceKHR surface_;
  Device device_;
  Swapchain swapchain_;
  ShaderModule vertex_shader_module_, fragment_shader_module_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_ENGINE_H_
