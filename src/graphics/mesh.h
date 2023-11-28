#ifndef SRC_GRAPHICS_MESH_H_
#define SRC_GRAPHICS_MESH_H_

#include <cassert>
#include <utility>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>

#include "graphics/buffer.h"
#include "graphics/device.h"

namespace gfx {

class Mesh {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec2 texture_coordinates;
    glm::vec3 normal;
  };

  Mesh(const Device& device, std::vector<Vertex> vertices, std::vector<std::uint32_t> indices)
      : vertices_{std::move(vertices)},
        indices_{std::move(indices)},
        vertex_buffer_{CreateDeviceLocalBuffer<Vertex>(device, vk::BufferUsageFlagBits::eVertexBuffer, vertices_)},
        index_buffer_{CreateDeviceLocalBuffer<std::uint32_t>(device, vk::BufferUsageFlagBits::eIndexBuffer, indices_)} {
    assert(indices_.size() % 3 == 0);
  }

  [[nodiscard]] const std::vector<Vertex>& vertices() const noexcept { return vertices_; }
  [[nodiscard]] const std::vector<std::uint32_t>& indices() const noexcept { return indices_; }
  [[nodiscard]] const glm::mat4& transform() const noexcept { return transform_; }

  void Translate(const glm::vec3& translation) { transform_ = glm::translate(transform_, translation); }
  void Rotate(const glm::vec3& axis, const float angle) { transform_ = glm::rotate(transform_, angle, axis); }
  void Scale(const glm::vec3& scale) { transform_ = glm::scale(transform_, scale); }

  void Render(const vk::CommandBuffer& command_buffer) const {
    command_buffer.bindVertexBuffers(0, *vertex_buffer_, static_cast<vk::DeviceSize>(0));
    command_buffer.bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);
    command_buffer.drawIndexed(static_cast<std::uint32_t>(indices_.size()), 1, 0, 0, 0);
  }

private:
  std::vector<Vertex> vertices_;
  std::vector<std::uint32_t> indices_;
  glm::mat4 transform_{1.0f};
  Buffer vertex_buffer_;
  Buffer index_buffer_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_MESH_H_
