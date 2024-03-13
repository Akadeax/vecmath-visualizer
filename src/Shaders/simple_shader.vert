#version 450
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
	mat4 model;
	mat4 normalMatrix;
} push;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} globalUbo;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.1;

void main()
{
	gl_Position = (globalUbo.proj * globalUbo.view * push.model) * vec4(position, 1.0);

	// Simple lambertian diffuse
	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
	float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);
	fragColor = lightIntensity * color;

	// debugPrintfEXT("MODEL: %f %f %f %f\n", ubo.model[3][0], ubo.model[3][1], ubo.model[3][2], ubo.model[3][3]);
}