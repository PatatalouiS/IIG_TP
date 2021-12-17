
#version 410 core

in vec3 normal;
in vec3 worldPosition;
in vec3 color;

out vec4 general_color;

uniform vec3 pos_key_light = vec3(-40, 175, 100);
uniform vec3 pos_fill_light = vec3(70, -25, 100);
uniform vec3 pos_back_light = vec3(20, 0, -20);
uniform vec3 light_color = vec3(1,0.92,0.87);

uniform bool useGPU;

void main() {
	vec3 light_ambient = 0.075 * light_color;
	vec3 norm = normal;
	vec3 dir_key_light = normalize(pos_key_light - worldPosition);
	vec3 dir_fill_light = normalize(pos_fill_light - worldPosition);
	vec3 dir_back_light = normalize(pos_back_light - worldPosition);
	vec3 key_light = max(dot(norm, dir_key_light), 0.0) * light_color * 1.75;
	vec3 fill_light = max(dot(norm, dir_fill_light), 0.0) * light_color * 0.5;
	vec3 back_light = max(dot(norm, dir_back_light), 0.0) * light_color * 2;
	vec3 f_color = color;

	if(useGPU) {
		vec3 temp = f_color;
		f_color.x = temp.y + 0.2;
		f_color.y = temp.z - 0.2;
		f_color.z = temp.x;
	}

	general_color = vec4((key_light + fill_light + back_light) * f_color * 0.8, 1.0);
}
