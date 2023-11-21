#version 460

layout(binding = 0, set = 0) uniform VertexTransforms {
  mat4 model_transform;
  mat4 view_transform;
  mat4 projection_transform;
} vertex_transforms;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordaintes;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec3 out_color;

void main() {
  gl_Position = vertex_transforms.projection_transform *
                vertex_transforms.view_transform *
                vertex_transforms.model_transform *
                vec4(position, 1.0);
  out_color = normal;
}
