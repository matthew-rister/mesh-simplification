#ifndef SRC_GRAPHICS_INCLUDE_GRAPHICS_MESH_H_
#define SRC_GRAPHICS_INCLUDE_GRAPHICS_MESH_H_

#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.hpp>

#include "graphics/buffer.h"

namespace gfx {
class Device;

/** \brief An indexed triangle mesh. */
class Mesh {
public:
  /** \brief Vertex attributes used in rendering. */
  struct Vertex {
    /** \brief The vertex position. */
    glm::vec3 position{0.0f};

    /** \brief The vertex texture coordinates. */
    glm::vec2 texture_coordinates{0.0f};

    /** \brief The vertex normal. */
    glm::vec3 normal{0.0f};
  };

  /**
   * \brief Initializes a mesh.
   * \param device The graphics device.
   * \param vertices The mesh vertices.
   * \param indices The mesh indices.
   * \param transform The model transformation matrix.
   * \remark The size of \p indices must be a multiple of three to define a valid triangle mesh.
   */
  Mesh(const Device& device,
       const std::vector<Vertex>& vertices,
       const std::vector<std::uint32_t>& indices,
       const glm::mat4& transform = glm::mat4{1.0f});

  /** \brief Gets the mesh vertices. */
  [[nodiscard]] const std::vector<Vertex>& vertices() const noexcept { return vertices_; }

  /** \brief Gets the mesh indices. */
  [[nodiscard]] const std::vector<std::uint32_t>& indices() const noexcept { return indices_; }

  /** \brief The model transformation matrix. */
  [[nodiscard]] const glm::mat4& transform() const noexcept { return transform_; }

  /**
   * \brief Translates the mesh along its coordinate axes.
   * \param translation The amount to translate the mesh in the x,y,z directions.
   */
  void Translate(const glm::vec3& translation) { transform_ = glm::translate(transform_, translation); }

  /**
   * \brief Rotates the mesh about an arbitrary axis.
   * \param axis The axis to rotate the mesh about.
   * \param angle The amount to rotate the mesh in degrees.
   */
  void Rotate(const glm::vec3& axis, const float angle) { transform_ = glm::rotate(transform_, angle, axis); }

  /**
   * \brief Scales the mesh along its coordinate axes.
   * \param scale The amount to scale the mesh in the x,y,z direction.
   */
  void Scale(const glm::vec3& scale) { transform_ = glm::scale(transform_, scale); }

  /**
   * \brief Renders the mesh.
   * \param command_buffer The command buffer to record draw commands to.
   */
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

#endif  // SRC_GRAPHICS_INCLUDE_GRAPHICS_MESH_H_
