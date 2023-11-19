#ifndef SRC_ENGINE_INCLUDE_ENGINE_MESH_H_
#define SRC_ENGINE_INCLUDE_ENGINE_MESH_H_

#include <array>

#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>

#include "engine/buffer.h"
#include "engine/device.h"

namespace gfx {

class Mesh {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
  };

  static constexpr vk::VertexInputBindingDescription kVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(Vertex),
      .inputRate = vk::VertexInputRate::eVertex};

  static constexpr std::array kVertexAttributeDescriptions{
      vk::VertexInputAttributeDescription{.location = 0,
                                          .binding = 0,
                                          .format = vk::Format::eR32G32B32Sfloat,
                                          .offset = offsetof(Vertex, position)},
      vk::VertexInputAttributeDescription{.location = 1,
                                          .binding = 0,
                                          .format = vk::Format::eR32G32B32Sfloat,
                                          .offset = offsetof(Vertex, color)}};

  static constexpr std::array kVertices{
      Vertex{.position = glm::vec3{-0.5f, -0.5f, 0.0f}, .color = glm::vec3{1.0f, 0.0f, 0.0f}},
      Vertex{.position = glm::vec3{-0.5f, 0.5f, 0.0f}, .color = glm::vec3{0.0f, 1.0f, 0.0f}},
      Vertex{.position = glm::vec3{0.5f, 0.5f, 0.0f}, .color = glm::vec3{0.0f, 0.0f, 1.0f}},
      Vertex{.position = glm::vec3{0.5f, -0.5f, 0.0f}, .color = glm::vec3{1.0f, 0.0f, 1.0f}}};

  static constexpr std::array<std::uint32_t, 6> kIndices{0, 1, 2, 0, 2, 3};

  explicit Mesh(const Device& device)
      : vertex_buffer_{CreateDeviceLocalBuffer<Vertex>(device, vk::BufferUsageFlagBits::eVertexBuffer, kVertices)},
        index_buffer_{CreateDeviceLocalBuffer<std::uint32_t>(device, vk::BufferUsageFlagBits::eIndexBuffer, kIndices)} {
  }

  void Render(const vk::CommandBuffer& command_buffer) const {
    command_buffer.bindVertexBuffers(0, *vertex_buffer_, static_cast<vk::DeviceSize>(0));
    command_buffer.bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);
    command_buffer.drawIndexed(static_cast<std::uint32_t>(index_buffer_.length()), 1, 0, 0, 0);
  }

private:
  Buffer<Vertex> vertex_buffer_;
  Buffer<std::uint32_t> index_buffer_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_MESH_H_
