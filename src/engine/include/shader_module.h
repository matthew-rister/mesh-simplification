#pragma once

#include <filesystem>

#include <vulkan/vulkan.hpp>

namespace gfx {

class ShaderModule {
public:
  ShaderModule(const vk::Device& device, vk::ShaderStageFlagBits shader_stage, const std::filesystem::path& filepath);

  [[nodiscard]] const vk::ShaderModule& operator*() const noexcept { return *shader_module_; }

  [[nodiscard]] vk::ShaderStageFlagBits stage() const noexcept { return shader_stage_; }

private:
  vk::UniqueShaderModule shader_module_;
  vk::ShaderStageFlagBits shader_stage_;
};

}  // namespace gfx
