#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_

#include <utility>

#include "graphics/camera.h"
#include "graphics/mesh.h"

namespace gfx {

class Scene {
public:
  Scene(const Camera& camera, Mesh&& mesh) : camera_{camera}, mesh_{std::move(mesh)} {}

  [[nodiscard]] const Camera& camera() const noexcept { return camera_; }
  [[nodiscard]] const Mesh& mesh() const noexcept { return mesh_; }

private:
  Camera camera_;
  Mesh mesh_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_SCENE_H_
