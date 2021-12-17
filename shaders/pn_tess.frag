
#version 410 core

in vec3 Normal_FS_in;
in vec3 worldPosition;
in vec3 position;
out vec4 general_color;

uniform vec3 pos_key_light = vec3(-40, 175, 100);
uniform vec3 pos_fill_light = vec3(70, -25, 100);
uniform vec3 pos_back_light = vec3(20, 0, -20);
uniform vec3 light_color = vec3(1,0.92,0.87);

void main() {
    vec3 light_ambient = 0.075 * light_color;
    vec3 norm_ = Normal_FS_in;
    vec3 dir_key_light = normalize(pos_key_light - worldPosition);
    vec3 dir_fill_light = normalize(pos_fill_light - worldPosition);
    vec3 dir_back_light = normalize(pos_back_light - worldPosition);
    vec3 key_light = max(dot(norm_, dir_key_light), 0.0) * light_color * 1.75;
    vec3 fill_light = max(dot(norm_, dir_fill_light), 0.0) * light_color * 0.5;
    vec3 back_light = max(dot(norm_, dir_back_light), 0.0) * light_color * 2;
	vec3 main_color = vec3(normalize(Normal_FS_in)*0.5+0.5);
    general_color = vec4((key_light + fill_light + back_light) * main_color * 0.8, 1.0);
}
