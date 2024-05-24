#include "graphics/glslang_compiler.h"

#include <cstring>
#include <format>
#include <iostream>
#include <memory>
#include <print>
#include <stdexcept>
#include <string_view>
#include <utility>

#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/resource_limits_c.h>

template <>
struct std::formatter<glslang_stage_t> : std::formatter<std::string_view> {
  [[nodiscard]] auto format(const glslang_stage_t glslang_stage, std::format_context& format_context) const {
    return std::formatter<std::string_view>::format(to_string(glslang_stage), format_context);
  }

private:
  static constexpr std::string_view to_string(const glslang_stage_t glslang_stage) noexcept {
    switch (glslang_stage) {
      // clang-format off
#define CASE(kGlslangStage) case kGlslangStage: return #kGlslangStage;  // NOLINT(cppcoreguidelines-macro-usage)
      CASE(GLSLANG_STAGE_VERTEX)
      CASE(GLSLANG_STAGE_TESSCONTROL)
      CASE(GLSLANG_STAGE_TESSEVALUATION)
      CASE(GLSLANG_STAGE_GEOMETRY)
      CASE(GLSLANG_STAGE_FRAGMENT)
      CASE(GLSLANG_STAGE_COMPUTE)
      CASE(GLSLANG_STAGE_RAYGEN)
      CASE(GLSLANG_STAGE_INTERSECT)
      CASE(GLSLANG_STAGE_ANYHIT)
      CASE(GLSLANG_STAGE_CLOSESTHIT)
      CASE(GLSLANG_STAGE_MISS)
      CASE(GLSLANG_STAGE_CALLABLE)
      CASE(GLSLANG_STAGE_TASK)
      CASE(GLSLANG_STAGE_MESH)
      CASE(GLSLANG_STAGE_COUNT)
#undef CASE
        // clang-format on
      default:
        std::unreachable();
    }
  }
};

namespace {

using UniqueGlslangShader = std::unique_ptr<glslang_shader_t, decltype(&glslang_shader_delete)>;
using UniqueGlslangProgram = std::unique_ptr<glslang_program_t, decltype(&glslang_program_delete)>;

constexpr auto kGlslangMessages =
// NOLINTBEGIN(hicpp-signed-bitwise)
#ifndef NDEBUG
    GLSLANG_MSG_DEBUG_INFO_BIT |
#endif
    GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT;
// NOLINTEND(hicpp-signed-bitwise)

std::ostream& operator<<(std::ostream& ostream, glslang_shader_t* const glslang_shader) {
  if (const auto* const glslang_shader_info_log = glslang_shader_get_info_log(glslang_shader);
      glslang_shader_info_log != nullptr && std::strlen(glslang_shader_info_log) > 0) {
    std::println(ostream, "{}", glslang_shader_info_log);
  }
#ifndef NDEBUG
  if (const auto* const glslang_shader_info_debug_log = glslang_shader_get_info_debug_log(glslang_shader);
      glslang_shader_info_debug_log != nullptr && std::strlen(glslang_shader_info_debug_log) > 0) {
    std::println(ostream, "{}", glslang_shader_info_debug_log);
  }
#endif
  return ostream;
}

std::ostream& operator<<(std::ostream& ostream, glslang_program_t* const glslang_program) {
  if (const auto* const glslang_program_info_log = glslang_program_get_info_log(glslang_program);
      glslang_program_info_log != nullptr && std::strlen(glslang_program_info_log) > 0) {
    std::println(ostream, "{}", glslang_program_info_log);
  }
#ifndef NDEBUG
  if (const auto* const glslang_program_info_debug_log = glslang_program_get_info_debug_log(glslang_program);
      glslang_program_info_debug_log != nullptr && std::strlen(glslang_program_info_debug_log) > 0) {
    std::println(ostream, "{}", glslang_program_info_debug_log);
  }
#endif
  return ostream;
}

UniqueGlslangShader CreateShader(const glslang_stage_t glslang_stage, const char* const glsl_source) {
  const glslang_input_t glslang_input{.language = GLSLANG_SOURCE_GLSL,
                                      .stage = glslang_stage,
                                      .client = GLSLANG_CLIENT_VULKAN,
                                      .client_version = GLSLANG_TARGET_VULKAN_1_3,
                                      .target_language = GLSLANG_TARGET_SPV,
                                      .target_language_version = GLSLANG_TARGET_SPV_1_6,
                                      .code = glsl_source,
                                      .default_version = 460,
                                      .default_profile = GLSLANG_NO_PROFILE,
                                      .force_default_version_and_profile = 0,
                                      .forward_compatible = 0,
                                      .messages = static_cast<glslang_messages_t>(kGlslangMessages),
                                      .resource = glslang_default_resource()};

  auto glslang_shader = UniqueGlslangShader{glslang_shader_create(&glslang_input), glslang_shader_delete};
  if (glslang_shader == nullptr) {
    throw std::runtime_error{
        std::format("Shader creation failed at {} with GLSL source:\n{}", glslang_stage, glsl_source)};
  }

  const auto glslang_shader_preprocess_result = glslang_shader_preprocess(glslang_shader.get(), &glslang_input);
  std::clog << glslang_shader.get();

  if (glslang_shader_preprocess_result == 0) {
    throw std::runtime_error{
        std::format("Shader preprocessing failed at {} with GLSL source:\n{}", glslang_stage, glsl_source)};
  }

  const auto glslang_shader_parse_result = glslang_shader_parse(glslang_shader.get(), &glslang_input);
  std::clog << glslang_shader.get();

  if (glslang_shader_parse_result == 0) {
    throw std::runtime_error{std::format("Shader parsing failed at {} with GLSL source:\n{}",
                                         glslang_stage,
                                         glslang_shader_get_preprocessed_code(glslang_shader.get()))};
  }

  return glslang_shader;
}

UniqueGlslangProgram CreateProgram(const glslang_stage_t glslang_stage, glslang_shader_t* const glslang_shader) {
  auto glslang_program = UniqueGlslangProgram{glslang_program_create(), glslang_program_delete};
  if (glslang_program == nullptr) {
    throw std::runtime_error{std::format("Shader program creation failed at {}", glslang_stage)};
  }
  glslang_program_add_shader(glslang_program.get(), glslang_shader);

  const auto glslang_program_link_result = glslang_program_link(glslang_program.get(), kGlslangMessages);
  std::clog << glslang_program.get();

  if (glslang_program_link_result == 0) {
    throw std::runtime_error{std::format("Shader program linking failed at {} with GLSL source:\n{}",
                                         glslang_stage,
                                         glslang_shader_get_preprocessed_code(glslang_shader))};
  }

  return glslang_program;
}

std::vector<std::uint32_t> GenerateSpirv(glslang_program_t* const glslang_program,
                                         const glslang_stage_t glslang_stage) {
  glslang_program_SPIRV_generate(glslang_program, glslang_stage);

  const auto spirv_size = glslang_program_SPIRV_get_size(glslang_program);
  if (spirv_size == 0) throw std::runtime_error{std::format("SPIR-V generation failed at {}", glslang_stage)};

  std::vector<std::uint32_t> spirv(spirv_size);
  glslang_program_SPIRV_get(glslang_program, spirv.data());

#ifndef NDEBUG
  if (const auto* const spirv_messages = glslang_program_SPIRV_get_messages(glslang_program);
      spirv_messages != nullptr && std::cmp_greater(std::strlen(spirv_messages), 0)) {
    std::println(std::clog, "{}", spirv_messages);
  }
#endif

  return spirv;
}

}  // namespace

namespace gfx {

GlslangCompiler::GlslangCompiler() {
  if (glslang_initialize_process() == 0) {
    throw std::runtime_error{"glslang initialization failed"};
  }
}

GlslangCompiler::~GlslangCompiler() noexcept { glslang_finalize_process(); }

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::vector<std::uint32_t> GlslangCompiler::Compile(const glslang_stage_t glslang_stage,
                                                    const char* const glsl_source) const {
  const auto glslang_shader = CreateShader(glslang_stage, glsl_source);
  const auto glslang_program = CreateProgram(glslang_stage, glslang_shader.get());
  return GenerateSpirv(glslang_program.get(), glslang_stage);
}

}  // namespace gfx
