#include "engine/shader_module.h"

#include <format>
#include <fstream>
#include <ios>
#include <stdexcept>

#include <glslang/Include/glslang_c_shader_types.h>

#include "engine/glslang_compiler.h"

namespace {

glslang_stage_t GetGlslangStage(const vk::ShaderStageFlagBits shader_stage) {
  switch (shader_stage) {  // NOLINT(clang-diagnostic-switch-enum)
    case vk::ShaderStageFlagBits::eVertex: return GLSLANG_STAGE_VERTEX;
    case vk::ShaderStageFlagBits::eTessellationControl: return GLSLANG_STAGE_TESSCONTROL;
    case vk::ShaderStageFlagBits::eTessellationEvaluation: return GLSLANG_STAGE_TESSEVALUATION;
    case vk::ShaderStageFlagBits::eGeometry: return GLSLANG_STAGE_GEOMETRY;
    case vk::ShaderStageFlagBits::eFragment: return GLSLANG_STAGE_FRAGMENT;
    case vk::ShaderStageFlagBits::eCompute: return GLSLANG_STAGE_COMPUTE;
    case vk::ShaderStageFlagBits::eRaygenKHR: return GLSLANG_STAGE_RAYGEN;
    case vk::ShaderStageFlagBits::eAnyHitKHR: return GLSLANG_STAGE_ANYHIT;
    case vk::ShaderStageFlagBits::eClosestHitKHR: return GLSLANG_STAGE_CLOSESTHIT;
    case vk::ShaderStageFlagBits::eMissKHR: return GLSLANG_STAGE_MISS;
    case vk::ShaderStageFlagBits::eIntersectionKHR: return GLSLANG_STAGE_INTERSECT;
    case vk::ShaderStageFlagBits::eCallableKHR: return GLSLANG_STAGE_CALLABLE;
    default: throw std::invalid_argument{std::format("Unsupported shader stage {}", vk::to_string(shader_stage))};
  }
}

std::string ReadFile(const std::filesystem::path& filepath) {
  if (std::ifstream ifs{filepath, std::ios::ate}; ifs.good()) {
    const std::streamsize size = ifs.tellg();
    std::string source(static_cast<std::size_t>(size), '\0');
    ifs.seekg(0, std::ios::beg);
    ifs.read(source.data(), size);
    return source;
  }
  throw std::runtime_error{std::format("Failed to open {}", filepath.string())};
}

}  // namespace

gfx::ShaderModule::ShaderModule(const vk::Device& device,
                                const vk::ShaderStageFlagBits shader_stage,
                                const std::filesystem::path& filepath) {
  const auto glsl = ReadFile(filepath);
  const auto spirv = GlslangCompiler::Get().Compile(GetGlslangStage(shader_stage), glsl.c_str());

  shader_module_ = device.createShaderModuleUnique(
      vk::ShaderModuleCreateInfo{.codeSize = spirv.size() * sizeof(decltype(spirv)::value_type),
                                 .pCode = spirv.data()});
}
