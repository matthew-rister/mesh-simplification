#ifndef GRAPHICS_INSTANCE_H_
#define GRAPHICS_INSTANCE_H_

#include <vulkan/vulkan.hpp>

namespace gfx {

class Instance {
public:
  Instance();

  [[nodiscard]] vk::Instance operator*() const noexcept { return *instance_; }

private:
  vk::UniqueInstance instance_;
};

}  // namespace gfx

#endif  // GRAPHICS_INSTANCE_H_
