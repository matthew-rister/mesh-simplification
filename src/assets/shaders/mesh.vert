#version 460

layout(push_constant) uniform MeshPushConstants {
  mat4 model_transform;
} mesh_push_constants;

layout(binding = 0, set = 0) uniform CameraUniformBuffer {
  mat4 view_transform;
  mat4 projection_transform;
} camera_uniform_buffer;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordaintes;
layout(location = 2) in vec3 normal;

layout(location = 0) out Vertex {
  vec3 position;
  vec3 normal;
} vertex;

void main() {
  const mat4 model_view_transform = camera_uniform_buffer.view_transform * mesh_push_constants.model_transform;
  const vec4 model_view_position = model_view_transform * vec4(position, 1.0);
  vertex.position = model_view_position.xyz;
  vertex.normal = normal;
  gl_Position = camera_uniform_buffer.projection_transform * model_view_position;
}
