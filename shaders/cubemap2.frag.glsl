#version 330 core
out vec4 color;

in vec3 normal2;
in vec3 position2;

uniform vec3 camera;
uniform samplerCube skybox;
uniform bool mode;

void main() {

	vec3 I = normalize(position2 - camera);
	vec3 R;

	if(mode) {
		R = reflect(I, normalize(normal2));
	}
	else {
		R = refract(I, normalize(normal2), 0.65);
	}

	color = vec4(texture(skybox, R).rgb, 1.0);
}
