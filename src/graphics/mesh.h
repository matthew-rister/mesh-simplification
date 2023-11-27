#ifndef SRC_GRAPHICS_MESH_H_
#define SRC_GRAPHICS_MESH_H_

#include <array>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>

#include "graphics/buffer.h"
#include "graphics/device.h"

// TODO(matthew-rister): vulkan pipeline constructs should not live here

namespace gfx {

class Mesh {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec2 texture_coordinates;
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
                                          .format = vk::Format::eR32G32Sfloat,
                                          .offset = offsetof(Vertex, texture_coordinates)},
      vk::VertexInputAttributeDescription{.location = 2,
                                          .binding = 0,
                                          .format = vk::Format::eR32G32B32Sfloat,
                                          .offset = offsetof(Vertex, normal)}};

  Mesh(const Device& device,
       const vk::ArrayProxy<const Vertex> vertices,
       const vk::ArrayProxy<const std::uint32_t> indices)
      : vertex_buffer_{CreateDeviceLocalBuffer(device, vk::BufferUsageFlagBits::eVertexBuffer, vertices)},
        index_buffer_{CreateDeviceLocalBuffer(device, vk::BufferUsageFlagBits::eIndexBuffer, indices)} {}

  [[nodiscard]] const glm::mat4& model_transform() const noexcept { return model_transform_; }

  void Translate(const float dx, const float dy, const float dz) {
    model_transform_ = glm::translate(model_transform_, glm::vec3{dx, dy, dz});
  }

  void Rotate(const glm::vec3& axis, const float angle) {
    model_transform_ = glm::rotate(model_transform_, angle, axis);
  }

  void Scale(const float sx, const float sy, const float sz) {
    model_transform_ = glm::scale(model_transform_, glm::vec3{sx, sy, sz});
  }

  void Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const {
    command_buffer.pushConstants<PushConstants>(pipeline_layout,
                                                vk::ShaderStageFlagBits::eVertex,
                                                0,
                                                PushConstants{.model_transform = model_transform_});
    command_buffer.bindVertexBuffers(0, *vertex_buffer_, static_cast<vk::DeviceSize>(0));
    command_buffer.bindIndexBuffer(*index_buffer_, 0, vk::IndexType::eUint32);
    command_buffer.drawIndexed(static_cast<std::uint32_t>(index_buffer_.length()), 1, 0, 0, 0);
  }

private:
  Buffer<Vertex> vertex_buffer_;
  Buffer<std::uint32_t> index_buffer_;
  glm::mat4 model_transform_{1.0f};
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_MESH_H_
