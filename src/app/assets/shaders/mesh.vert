#version 460

layout(push_constant) uniform VertexTransforms {
  mat4 model_view_transform; // model-view transform assumed to be an orthogonal matrix
  mat4 projection_transform;
} vertex_transforms;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec3 normal;

layout(location = 0) out Vertex {
  vec3 position;
  vec3 normal;
} vertex;

void main() {
  const vec4 model_view_position = vertex_transforms.model_view_transform * vec4(position, 1.0);
  const mat3 normal_transform = mat3(vertex_transforms.model_view_transform);
  vertex.position = model_view_position.xyz;
  vertex.normal = normalize(normal_transform * normal);
  gl_Position = vertex_transforms.projection_transform * model_view_position;
}
