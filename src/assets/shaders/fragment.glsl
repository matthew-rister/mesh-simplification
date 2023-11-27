#version 460

layout(location = 0) in Vertex {
  vec3 normal;
} vertex;

layout(location = 0) out vec4 fragment_color;

void main() {
  fragment_color = vec4(vertex.normal, 1.0);
}
