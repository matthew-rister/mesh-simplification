#ifndef SRC_ENGINE_INCLUDE_ENGINE_MODEL_H_
#define SRC_ENGINE_INCLUDE_ENGINE_MODEL_H_

#include <filesystem>
#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.hpp>

namespace gfx {
class Device;
class Mesh;

struct Node {
  std::vector<Mesh> meshes;
  std::vector<std::unique_ptr<Node>> children;
  glm::mat4 transform{1.0f};
};

class Model {
public:
  Model(const Device& device, const std::filesystem::path& filepath);

  void Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const;

  void Translate(const float dx, const float dy, const float dz) const;
  void Rotate(const glm::vec3& axis, const float angle) const;
  void Scale(const float sx, const float sy, const float sz) const;

private:
  std::unique_ptr<Node> root_node_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_MODEL_H_
