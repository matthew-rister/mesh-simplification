#version 460

layout(location = 0) out vec3 vertex_color;

vec2 vertex_positions[] = {
  {0.0, -0.5},
  {-0.5, 0.5},
  {0.5, 0.5}
};

vec3 vertex_colors[] = {
  {1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0}
};

void main() {
  gl_Position = vec4(vertex_positions[gl_VertexIndex], 0.0, 1.0);
  vertex_color = vertex_colors[gl_VertexIndex];
}
