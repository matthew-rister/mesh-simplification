#include "graphics/scene.h"

#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "geometry/mesh_simplifier.h"
#include "graphics/material.h"
#include "graphics/obj_loader.h"
#include "graphics/shader_program.h"
#include "graphics/window.h"

using namespace glm;
using namespace qem;
using namespace std;

namespace {

	struct ViewFrustrum {
		float field_of_view_y;
		float z_near;
		float z_far;
	} constexpr kViewFrustrum = {
		.field_of_view_y = radians(45.f),
		.z_near = .1f,
		.z_far = 100.f
	};

	struct PointLight {
		vec3 position; // defined in view-space coordinates
		vec3 color;
		vec3 attenuation;
	} constexpr kPointLights[] = {
		{
			.position = vec3{1.f, 1.f, 0.f},
			.color = vec3{1.f},
			.attenuation = vec3{0.f, 0.f, 1.f}
		},
		{
			.position = vec3{-1.f, 0.f, 1.f},
			.color = vec3{1.f},
			.attenuation = vec3{0.f, 0.f, 1.f}
		},
		{
			.position = vec3{0.f, 3.f, -2.f},
			.color = vec3{1.f},
			.attenuation = vec3{0.f, 0.f, 1.f}
		}
	};

	void SetMaterial(ShaderProgram& shader_program) {
		const auto [ambient, diffuse, specular, shininess] = Material::FromType(MaterialType::kJade);
		shader_program.SetUniform("material.ambient", ambient);
		shader_program.SetUniform("material.diffuse", diffuse);
		shader_program.SetUniform("material.specular", specular);
		shader_program.SetUniform("material.shininess", shininess * 128.f);
	}

	void SetPointLights(ShaderProgram& shader_program) {
		constexpr int kPointLightsSize = sizeof kPointLights / sizeof(PointLight);
		shader_program.SetUniform("point_lights_size", kPointLightsSize);

		for (auto i = 0; i < kPointLightsSize; ++i) {
			const auto& [position, color, attenuation] = kPointLights[i];
			shader_program.SetUniform(format("point_lights[{}].position", i), position);
			shader_program.SetUniform(format("point_lights[{}].color", i), color);
			shader_program.SetUniform(format("point_lights[{}].attenuation", i), attenuation);
		}
	}

	void SetViewTransforms(const Window& window, const Camera& camera, const Mesh& mesh, ShaderProgram& shader_program) {
		static auto prev_aspect_ratio = 0.f;

		if (const auto aspect_ratio = window.AspectRatio(); prev_aspect_ratio != aspect_ratio && aspect_ratio > 0.f) {
			const auto [field_of_view_y, z_near, z_far] = kViewFrustrum;
			const auto projection_transform = perspective(field_of_view_y, aspect_ratio, z_near, z_far);
			shader_program.SetUniform("projection_transform", projection_transform);
			prev_aspect_ratio = aspect_ratio;
		}

		const auto model_view_transform = camera.GetViewTransform() * mesh.model_transform();
		shader_program.SetUniform("model_view_transform", model_view_transform);
	}

	void HandleContinuousInput(const Window& window, Camera& camera, Mesh& mesh, const float delta_time) {
		static optional<dvec2> prev_cursor_position;

		if (const auto cursor_position = window.CursorPosition(); window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
			if (prev_cursor_position) {
				const auto rotation_step = .2f * delta_time;
				const auto cursor_delta = rotation_step * static_cast<vec2>(cursor_position - *prev_cursor_position);
				camera.Rotate(-cursor_delta.x, -cursor_delta.y);
			}
			prev_cursor_position = cursor_position;
		} else if (window.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
			if (prev_cursor_position) {
				const auto translate_step = .2f * delta_time;
				const auto cursor_delta = translate_step * static_cast<vec2>(cursor_position - *prev_cursor_position);
				const auto view_model_inv = inverse(camera.GetViewTransform() * mesh.model_transform());
				mesh.Translate(view_model_inv * vec4{cursor_delta.x, -cursor_delta.y, 0.f, 0.f});
			}
			prev_cursor_position = cursor_position;
		} else if (prev_cursor_position) {
			prev_cursor_position = nullopt;
		}
	}
}

Scene::Scene(Window* const window)
	: window_{window},
	  camera_{vec3{-.2f, .3f, 0.f}, 2.f, 0.f, 0.f},
	  mesh_{obj_loader::LoadMesh("models/bunny.obj")},
	  shader_program_{"shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl"} {

	window_->OnKeyPress([this](const auto key_code) {
		if (key_code == GLFW_KEY_S) {
			mesh_ = mesh::Simplify(mesh_, .5f);
		}
	});

	window_->OnScroll([this](const auto /*x_offset*/, const auto y_offset) {
		constexpr auto kScaleStep = .02f;
		const auto sign = static_cast<float>(y_offset > 0) - static_cast<float>(y_offset < 0);
		mesh_.Scale(vec3{1.f + sign * kScaleStep});
	});

	shader_program_.Enable();
	SetMaterial(shader_program_);
	SetPointLights(shader_program_);

	mesh_.Scale(vec3{.35f});
}

void Scene::Render(const float delta_time) {
	glClearColor(.1f, .1f, .1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	HandleContinuousInput(*window_, camera_, mesh_, delta_time);
	SetViewTransforms(*window_, camera_, mesh_, shader_program_);
	mesh_.Render();
}
