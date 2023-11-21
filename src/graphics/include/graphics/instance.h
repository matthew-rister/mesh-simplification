#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_INSTANCE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_INSTANCE_H_

#include <vulkan/vulkan.hpp>

namespace gfx {

class Instance {
public:
  Instance();

  [[nodiscard]] const vk::Instance& operator*() const noexcept { return *instance_; }

private:
  vk::UniqueInstance instance_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_INSTANCE_H_
