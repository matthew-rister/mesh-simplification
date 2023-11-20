#include "engine/model.h"

#include <cassert>
#include <cstdint>
#include <ranges>
#include <span>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/device.h"
#include "engine/mesh.h"

// TODO(matthew-rister): review push constants
// TODO(matthew-rister): fix copy assets

namespace {

template <glm::length_t N>
  requires(0 < N && N <= 3)
glm::vec<N, float> GetVec(const std::span<const aiVector3D> data, const size_t index) {
  glm::vec<N, float> v{};
  if (index < data.size()) {
    for (glm::length_t component = 0; component < N; ++component) {
      v[component] = data[index][static_cast<std::uint32_t>(component)];
    }
  }
  return v;
}

std::vector<gfx::Mesh::Vertex> GetVertices(const aiMesh& mesh) {
  return std::views::iota(0u, mesh.mNumVertices)
         | std::views::transform(
             [positions = std::span{mesh.mVertices, mesh.HasPositions() ? mesh.mNumVertices : 0},
              normals = std::span{mesh.mNormals, mesh.HasNormals() ? mesh.mNumVertices : 0}](const auto index) {
               return gfx::Mesh::Vertex{.position = GetVec<3>(positions, index), .normal = GetVec<3>(normals, index)};
             })
         | std::ranges::to<std::vector>();
}

std::vector<std::uint32_t> GetIndices(const aiMesh& mesh) {
  return std::span{mesh.mFaces, mesh.HasFaces() ? mesh.mNumFaces : 0}  //
         | std::views::transform([](const auto& face) {
             return std::span{face.mIndices, face.mNumIndices};
           })
         | std::views::join  //
         | std::ranges::to<std::vector>();
}

glm::mat4 GetTransform(const aiNode& node) {
  const auto& transform = node.mTransformation;
  const std::span x{transform[0], 4};
  const std::span y{transform[1], 4};
  const std::span z{transform[2], 4};
  const std::span w{transform[3], 4};

  // clang-format off
  return glm::mat4{x[0], y[0], z[0], w[0],
                   x[1], y[1], z[1], w[1],
                   x[2], y[2], z[2], w[2],
                   x[3], y[3], z[3], w[3]};
  // clang-format on
}

std::unique_ptr<gfx::Node> ImportNode(const gfx::Device& device, const aiScene& scene, const aiNode& node) {
  auto node_meshes = std::span{node.mMeshes, node.mNumMeshes}
                     | std::views::transform(
                         [&device, scene_meshes = std::span{scene.mMeshes, scene.mNumMeshes}](const auto mesh_index) {
                           assert(mesh_index < scene_meshes.size());
                           assert(scene_meshes[mesh_index] != nullptr);
                           const auto& mesh = *scene_meshes[mesh_index];
                           return gfx::Mesh{device, GetVertices(mesh), GetIndices(mesh)};
                         })
                     | std::ranges::to<std::vector>();

  auto node_children = std::span{node.mChildren, node.mNumChildren}
                       | std::views::transform(
                           [&device, &scene](const auto* child_node) { return ImportNode(device, scene, *child_node); })
                       | std::ranges::to<std::vector>();

  return std::make_unique<gfx::Node>(std::move(node_meshes), std::move(node_children), GetTransform(node));
}

void RenderNode(const gfx::Node& node,
                const vk::CommandBuffer& command_buffer,
                const vk::PipelineLayout& pipeline_layout,
                const glm::mat4& parent_transform = glm::mat4{1.0f}) {
  const auto transform = parent_transform * node.transform;

  for (const auto& mesh : node.meshes) {
    mesh.Render(command_buffer, pipeline_layout, transform);
  }

  for (const auto& child_node : node.children) {
    RenderNode(*child_node, command_buffer, pipeline_layout, transform);
  }
}

}  // namespace

gfx::Model::Model(const Device& device, const std::filesystem::path& filepath) {
  Assimp::Importer importer;
  std::uint32_t import_flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate;

#ifdef NDEBUG
  import_flags |= aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;
#else
  import_flags |= aiProcess_ValidateDataStructure;
  Assimp::DefaultLogger::create(ASSIMP_DEFAULT_LOG_NAME, Assimp::Logger::DEBUGGING);
#endif

  const auto* scene = importer.ReadFile(filepath.string(), import_flags);
  if (scene == nullptr) throw std::runtime_error{importer.GetErrorString()};

  root_node_ = ImportNode(device, *scene, *scene->mRootNode);
}
void gfx::Model::Render(const vk::CommandBuffer& command_buffer, const vk::PipelineLayout& pipeline_layout) const {
  RenderNode(*root_node_, command_buffer, pipeline_layout);
}

void gfx::Model::Translate(const float dx, const float dy, const float dz) const {
  auto& root_transform = root_node_->transform;
  root_transform = glm::translate(root_transform, glm::vec3{dx, dy, dz});
}

void gfx::Model::Rotate(const glm::vec3& axis, const float angle) const {
  auto& root_transform = root_node_->transform;
  root_transform = glm::rotate(root_transform, angle, axis);
}

void gfx::Model::Scale(const float sx, const float sy, const float sz) const {
  auto& root_transform = root_node_->transform;
  root_transform = glm::scale(root_transform, glm::vec3{sx, sy, sz});
}
