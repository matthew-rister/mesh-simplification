#include "graphics/image.h"

#include "graphics/device.h"

namespace {

vk::UniqueImage CreateImage(const vk::Device& device,
                            const vk::Format format,
                            const vk::Extent2D& extent,
                            const vk::ImageUsageFlags& image_usage_flags) {
  return device.createImageUnique(
      vk::ImageCreateInfo{.imageType = vk::ImageType::e2D,
                          .format = format,
                          .extent = vk::Extent3D{.width = extent.width, .height = extent.height, .depth = 1},
                          .mipLevels = 1,
                          .arrayLayers = 1,
                          .usage = image_usage_flags});
}

vk::UniqueImageView CreateImageView(const vk::Device& device,
                                    const vk::Image& image,
                                    const vk::Format format,
                                    const vk::ImageAspectFlags& image_aspect_flags) {
  return device.createImageViewUnique(
      vk::ImageViewCreateInfo{.image = image,
                              .viewType = vk::ImageViewType::e2D,
                              .format = format,
                              .subresourceRange = vk::ImageSubresourceRange{.aspectMask = image_aspect_flags,  //
                                                                            .levelCount = 1,
                                                                            .layerCount = 1}});
}

}  // namespace

gfx::Image::Image(const Device& device,
                  const vk::Format format,
                  const vk::Extent2D& extent,
                  const vk::ImageUsageFlags& image_usage_flags,
                  const vk::ImageAspectFlags& image_aspect_flags,
                  const vk::MemoryPropertyFlags& memory_property_flags)
    : image_{CreateImage(*device, format, extent, image_usage_flags)},
      memory_{device, device->getImageMemoryRequirements(*image_), memory_property_flags},
      format_{format} {
  device->bindImageMemory(*image_, *memory_, 0);
  image_view_ = CreateImageView(*device, *image_, format, image_aspect_flags);
}
