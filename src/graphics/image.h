#ifndef GRAPHICS_IMAGE_H_
#define GRAPHICS_IMAGE_H_

#include <vulkan/vulkan.hpp>

#include "graphics/memory.h"

namespace gfx {
class Device;

class Image {
public:
  Image(const Device& device,
        vk::Format format,
        vk::Extent2D extent,
        vk::SampleCountFlagBits sample_count,
        vk::ImageUsageFlags image_usage_flags,
        vk::ImageAspectFlags image_aspect_flags,
        vk::MemoryPropertyFlags memory_property_flags);

  [[nodiscard]] vk::ImageView image_view() const noexcept { return *image_view_; }
  [[nodiscard]] vk::Format format() const noexcept { return format_; }

private:
  vk::UniqueImage image_;
  vk::UniqueImageView image_view_;
  Memory memory_;
  vk::Format format_;
};

}  // namespace gfx

#endif  // GRAPHICS_IMAGE_H_
