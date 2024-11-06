#ifndef GRAPHICS_GLSLANG_COMPILER_H_
#define GRAPHICS_GLSLANG_COMPILER_H_

#include <cstdint>
#include <string>
#include <vector>

#include <glslang/Include/glslang_c_shader_types.h>

namespace gfx::glslang {

std::vector<std::uint32_t> Compile(glslang_stage_t glslang_stage, const std::string& glsl_source);

}  // namespace gfx::glslang

#endif  // GRAPHICS_GLSLANG_COMPILER_H_
