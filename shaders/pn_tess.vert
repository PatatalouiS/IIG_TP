#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 worldPosition;

void main() {
	worldPosition = (model * vec4(position, 1.0f)).xyz;
	normal = normalize((model * vec4(inormal, 0.0f)).xyz);
}

