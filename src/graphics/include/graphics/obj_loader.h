#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_OBJ_LOADER_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_OBJ_LOADER_H_

#include <filesystem>

namespace gfx {
class Device;
class Mesh;

namespace obj_loader {

Mesh LoadMesh(const Device& device, const std::filesystem::path& filepath);

}  // namespace obj_loader
}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_OBJ_LOADER_H_
