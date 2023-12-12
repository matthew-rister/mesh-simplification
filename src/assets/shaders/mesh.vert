#version 460

layout(push_constant) uniform VertexTransforms {
  mat4 model_view_transform;
  mat4 projection_transform;
} vertex_transforms;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordaintes;
layout(location = 2) in vec3 normal;

layout(location = 0) out Vertex {
  vec3 position;
  vec3 normal;
} vertex;

void main() {
  const mat4 model_view_transform = vertex_transforms.model_view_transform;
  const mat3 normal_transform = mat3(model_view_transform); // model-view transform is an orthogonal matrix
  const mat4 projection_transform = vertex_transforms.projection_transform;
  const vec4 model_view_position = model_view_transform * vec4(position, 1.0);
  vertex.position = model_view_position.xyz;
  vertex.normal = normalize(normal_transform * normal);
  gl_Position = projection_transform * model_view_position;
}
