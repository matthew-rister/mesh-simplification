#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_GLSLANG_COMPILER_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_GLSLANG_COMPILER_H_

#include <cstdint>
#include <vector>

#include <glslang/Include/glslang_c_shader_types.h>

namespace gfx {

class GlslangCompiler {
public:
  [[nodiscard]] static const GlslangCompiler& Get() {
    static const GlslangCompiler kInstance;
    return kInstance;
  }

  GlslangCompiler(const GlslangCompiler&) = delete;
  GlslangCompiler& operator=(const GlslangCompiler&) = delete;

  GlslangCompiler(GlslangCompiler&&) noexcept = delete;
  GlslangCompiler& operator=(GlslangCompiler&&) noexcept = delete;

  ~GlslangCompiler() noexcept;

  [[nodiscard]] std::vector<std::uint32_t> Compile(glslang_stage_t glslang_stage, const char* glsl_source) const;

private:
  GlslangCompiler();
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_GLSLANG_COMPILER_H_
