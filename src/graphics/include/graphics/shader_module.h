#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_SHADER_MODULE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_SHADER_MODULE_H_

#include <filesystem>

#include <vulkan/vulkan.hpp>

namespace gfx {

class ShaderModule {
public:
  ShaderModule(const vk::Device device,
               const vk::ShaderStageFlagBits shader_stage,
               const std::filesystem::path& filepath);

  [[nodiscard]] vk::ShaderModule operator*() const noexcept { return *shader_module_; }

private:
  vk::UniqueShaderModule shader_module_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_SHADER_MODULE_H_
