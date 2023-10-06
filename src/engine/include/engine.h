#pragma once

#include "instance.h"

#include <vulkan/vulkan.hpp>

namespace gfx {
class Window;

class Engine {
public:
  explicit Engine(const Window&);

private:
  Instance instance_;
  vk::UniqueSurfaceKHR surface_;
};

}  // namespace gfx
