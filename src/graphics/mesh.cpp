#include "graphics/mesh.h"

#include <cassert>

#include "graphics/device.h"

namespace {

template <typename T>
gfx::Buffer CreateDeviceLocalBuffer(const gfx::Device& device,
                                    const vk::BufferUsageFlags buffer_usage_flags,
                                    const std::vector<T>& data) {
  const auto size_bytes = sizeof(T) * data.size();

  gfx::Buffer host_visible_buffer{device,
                                  size_bytes,
                                  vk::BufferUsageFlagBits::eTransferSrc,
                                  vk::MemoryPropertyFlagBits::eHostVisible};
  host_visible_buffer.Copy<const T>(data);

  gfx::Buffer device_local_buffer{device,
                                  size_bytes,
                                  buffer_usage_flags | vk::BufferUsageFlagBits::eTransferDst,
                                  vk::MemoryPropertyFlagBits::eDeviceLocal};
  device_local_buffer.Copy(device, host_visible_buffer);

  return device_local_buffer;
}

}  // namespace

namespace gfx {

Mesh::Mesh(const Device& device,
           const std::vector<Vertex>& vertices,
           const std::vector<std::uint32_t>& indices,
           const glm::mat4& transform)
    : vertices_{vertices},
      indices_{indices},
      transform_{transform},
      vertex_buffer_{CreateDeviceLocalBuffer(device, vk::BufferUsageFlagBits::eVertexBuffer, vertices_)},
      index_buffer_{CreateDeviceLocalBuffer(device, vk::BufferUsageFlagBits::eIndexBuffer, indices_)} {
  assert(indices_.size() % 3 == 0);
}

}  // namespace gfx
