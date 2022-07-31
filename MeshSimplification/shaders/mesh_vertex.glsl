#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_coordinates;
layout (location = 2) in vec3 normal;

uniform mat4 projection_transform;
uniform mat4 model_view_transform;

out Vertex {
	vec4 position;
	vec3 normal;
} vertex;

void main() {
	// generally, normals should be transformed by the upper 3x3 inverse transpose of the model-view matrix. In this context,
	// it is sufficient to use the model-view matrix to transform normals because the mesh is only transformed by rotations
	// and translations (which are orthogonal matrices with the property that their inverse is equal to their transpose) in
	// addition to uniform unscaling which is undone when the transformed normal is renomalized.
	vertex.normal = normalize(mat3(model_view_transform) * normal);
	vertex.position = model_view_transform * vec4(position, 1.);
	gl_Position = projection_transform * vertex.position;
}
