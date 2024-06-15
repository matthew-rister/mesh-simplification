#ifndef GRAPHICS_MESH_H_
#define GRAPHICS_MESH_H_

#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>

#include "graphics/buffer.h"

namespace gfx {
class Device;

class Mesh {
public:
  struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec2 texture_coordinates{0.0f};
    glm::vec3 normal{0.0f};
  };

  Mesh(const Device& device,
       const std::vector<Vertex>& vertices,
       const std::vector<std::uint32_t>& indices,
       const glm::mat4& transform = glm::mat4{1.0f});

  [[nodiscard]] const std::vector<Vertex>& vertices() const noexcept { return vertices_; }
  [[nodiscard]] const std::vector<std::uint32_t>& indices() const noexcept { return indices_; }
  [[nodiscard]] const glm::mat4& transform() const noexcept { return transform_; }

  void Translate(const glm::vec3& translation) { transform_ = glm::translate(transform_, translation); }
  void Rotate(const glm::vec3& axis, const float angle) { transform_ = glm::rotate(transform_, angle, axis); }
  void Scale(const glm::vec3& scale) { transform_ = glm::scale(transform_, scale); }

  void Render(const vk::CommandBuffer command_buffer) const {
    command_buffer.bindVertexBuffers(0, *vertex_buffer_, static_cast<vk::DeviceSize>(0));
    command_buffer.bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);
    command_buffer.drawIndexed(static_cast<std::uint32_t>(indices_.size()), 1, 0, 0, 0);
  }

private:
  std::vector<Vertex> vertices_;
  std::vector<std::uint32_t> indices_;
  glm::mat4 transform_;
  Buffer vertex_buffer_;
  Buffer index_buffer_;
};

}  // namespace gfx

#endif  // GRAPHICS_MESH_H_
