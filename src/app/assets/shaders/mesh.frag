#version 460

layout(location = 0) in Vertex {
  vec3 position;
  vec3 normal;
} vertex;

layout(location = 0) out vec4 fragment_color;

// TODO(#2): store point light and material data in a uniform buffer
struct PointLight {
  vec3 position;
  vec3 color;
} kPointLights[3] = {
  {{1.0f, 1.0f, 0.0f}, {0.0, 1.0, 1.0}},
  {{-1.0f, 0.0f, 1.0f}, {1.0, 1.0, 1.0}},
  {{0.0f, 3.0f, -2.0f}, {1.0, 1.0, 1.0}}
};

struct Material {
 vec3 ambient;
 vec3 diffuse;
 vec3 specular;
 float reflectance;
} kMaterial = {
  {0.135, 0.2225, 0.1575},
  {0.54, 0.89, 0.63},
  {0.316228, 0.316228, 0.316228},
  12.8
};

void main() {
  const vec3 normal = normalize(cross(dFdx(vertex.position), -dFdy(vertex.position)));
  const vec3 view_direction = normalize(-vertex.position);
  fragment_color = vec4(kMaterial.ambient, 1.0);

  for (int i = 0; i < kPointLights.length(); ++i) {
    const PointLight point_light = kPointLights[i];

    vec3 light_direction = point_light.position - vertex.position;
    const float light_distance = length(light_direction);
    const float attenuation = 1.0 / max(light_distance * light_distance, 1.0);
    light_direction = light_direction / light_distance;

    const float diffuse_intensity = max(dot(normal, light_direction), 0.0);
    const vec3 diffuse_color = diffuse_intensity * kMaterial.diffuse;

    const vec3 reflect_direction = reflect(-light_direction, normal);
    const float specular_intensity = pow(max(dot(view_direction, reflect_direction), 0.0), kMaterial.reflectance);
    const vec3 specular_color = specular_intensity * kMaterial.specular;

    fragment_color += vec4(attenuation * point_light.color * (diffuse_color + specular_color), 0.0);
  }
}
