#pragma once

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