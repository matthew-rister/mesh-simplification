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
    glm::vec3 normal;
  };

  struct PushConstants {
    glm::mat4 model_transform;
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
                                          .offset = offsetof(Vertex, normal)}};

  Mesh(const Device& device,
       const vk::ArrayProxy<const Vertex>& vertices,
       const vk::ArrayProxy<const std::uint32_t>& indices)
      : vertex_buffer_{CreateDeviceLocalBuffer(device, vk::BufferUsageFlagBits::eVertexBuffer, vertices)},
        index_buffer_{CreateDeviceLocalBuffer(device, vk::BufferUsageFlagBits::eIndexBuffer, indices)} {}

  void Render(const vk::CommandBuffer& command_buffer,
              const vk::PipelineLayout& pipeline_layout,
              const glm::mat4& transform) const {
    command_buffer.pushConstants<PushConstants>(pipeline_layout,
                                                vk::ShaderStageFlagBits::eVertex,
                                                0,
                                                PushConstants{.model_transform = transform});
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
