#ifndef SRC_ENGINE_INCLUDE_ENGINE_SWAPCHAIN_H_
#define SRC_ENGINE_INCLUDE_ENGINE_SWAPCHAIN_H_

#include <experimental/generator>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace gfx {
class Device;
class Window;

class Swapchain {
  template <typename T>
  using generator = std::experimental::generator<T>;

public:
  Swapchain(const Device& device, const Window& window, const vk::SurfaceKHR& surface);

  [[nodiscard]] const vk::SwapchainKHR& operator*() const noexcept { return *swapchain_; }
  [[nodiscard]] const vk::SwapchainKHR* operator->() const noexcept { return &(*swapchain_); }

  [[nodiscard]] vk::Format image_format() const noexcept { return image_format_; }
  [[nodiscard]] const vk::Extent2D& image_extent() const noexcept { return image_extent_; }

  [[nodiscard]] generator<vk::ImageView> image_views() const {
    for (const auto& image_view : image_views_) {
      co_yield *image_view;
    }
  }

private:
  vk::UniqueSwapchainKHR swapchain_;
  vk::Format image_format_{};
  vk::Extent2D image_extent_;
  std::vector<vk::UniqueImageView> image_views_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_SWAPCHAIN_H_
