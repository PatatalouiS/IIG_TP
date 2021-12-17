#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 3) in vec2 weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 rootTransform;
uniform mat4 childTransform;
uniform bool useGPU;

out vec3 normal;
out vec3 worldPosition;
out vec3 color;

void main() {
	vec4 vpos;

	if(useGPU) {
		mat4 weightedTransform = (rootTransform * weights.x) + (childTransform * weights.y);
		vec4 animPosition = weightedTransform * vec4(position, 1.0f);
		vpos = (model * animPosition);
	}
	else {
		vpos = (model * vec4(position, 1.0f));
	}

	gl_Position = projection * view * model * vpos;
	worldPosition = vpos.xyz;
	color = vec3(1, 0, 0) * weights.x + vec3(0, 1, 0) * weights.y;
	normal = normalize((model * vec4(inormal, 0.0f)).xyz);
}

