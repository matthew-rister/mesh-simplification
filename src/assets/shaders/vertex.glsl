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
  vec3 normal;
} vertex;

void main() {
  gl_Position = camera_uniform_buffer.projection_transform *
                camera_uniform_buffer.view_transform *
                mesh_push_constants.model_transform *
                vec4(position, 1.0);
  vertex.normal = normal;
}
