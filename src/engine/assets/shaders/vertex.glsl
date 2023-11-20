#version 460

layout(binding = 0, set = 0) uniform CameraTransforms {
	mat4 view_transform;
	mat4 projection_transform;
} camera_transforms;

layout(push_constant) uniform PushConstants {
	mat4 model_transform;
} push_constants;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

layout(location = 0) out vec3 out_color;

void main() {
	gl_Position = camera_transforms.projection_transform *
								camera_transforms.view_transform *
								push_constants.model_transform *
								vec4(in_position, 1.0);
	out_color = in_normal;
}
