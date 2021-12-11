#version 330 core
out vec4 color;

in vec3 tex;

uniform samplerCube skybox;

void main() {
	color = texture(skybox, tex);
}
