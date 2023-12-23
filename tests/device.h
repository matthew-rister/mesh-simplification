#ifndef TESTS_DEVICE_H_
#define TESTS_DEVICE_H_

#include <vulkan/vulkan.hpp>

#include "graphics/device.h"
#include "graphics/instance.h"
#include "graphics/window.h"

namespace gfx::test {

class Device {
public:
  static const gfx::Device& Get() {
    static const Device kTestDevice;
    return kTestDevice.device_;
  }

private:
  Device() = default;

  Window window_{"Mesh Simplification Tests", {.width = 1920, .height = 1080}};
  Instance instance_;
  vk::UniqueSurfaceKHR surface_{window_.CreateSurface(*instance_)};
  gfx::Device device_{*instance_, *surface_};
};

}  // namespace gfx::test

#endif  // TESTS_DEVICE_H_
