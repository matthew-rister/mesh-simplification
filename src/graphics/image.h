#ifndef SRC_GRAPHICS_IMAGE_H_
#define SRC_GRAPHICS_IMAGE_H_

#include <vulkan/vulkan.hpp>

#include "graphics/memory.h"

namespace gfx {
class Device;

class Image {
public:
  Image(const Device& device,
        const vk::Format format,
        const vk::Extent2D& extent,
        const vk::ImageUsageFlags& image_usage_flags,
        const vk::ImageAspectFlags& image_aspect_flags,
        const vk::MemoryPropertyFlags& memory_property_flags);

  [[nodiscard]] const vk::ImageView& image_view() const noexcept { return *image_view_; }
  [[nodiscard]] vk::Format format() const noexcept { return format_; }

private:
  vk::UniqueImage image_;
  vk::UniqueImageView image_view_;
  Memory memory_;
  vk::Format format_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_IMAGE_H_
