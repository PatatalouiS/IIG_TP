#version 330 core
layout (location = 0) in vec3 position1;
layout (location = 1) in vec3 normal1;

out vec3 normal2;
out vec3 position2;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	normal2 = mat3(transpose(inverse(model))) * normal1;
	position2 = vec3(model * vec4(position1, 1.0));
	gl_Position = projection * view * model *  vec4(position1, 1.0);
}
