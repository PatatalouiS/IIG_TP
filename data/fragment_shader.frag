#version 410 core
in vec3 normal;
in vec3 glpos;
in vec3 fragment_position;
out vec4 general_color;
uniform vec3 light_position = vec3(3, 0, 3);
uniform vec3 light_color = vec3(1,1,1);

void main()
{
    vec3 light_ambient = 0.25 * light_color;
    vec3 norm_ = normalize(normal);
    vec3 light_direction = normalize(light_position - fragment_position);
    vec3 light_diffuse = max(dot(norm_, light_direction), 0.0) * light_color;
    //color = vec4(vec3(clamp(dot(normali1ze(normal), vec3(0,0,1)), 0, 1)), 1.0);
    //float error = (1 - length(glpos)) * 150;
    //color = vec4(0.5 + error, 0, 0, 1);
    vec3 color = vec3(normalize(normal)*0.5+0.5);
    //color = vec4(1, 0, 0, 1);
    general_color = vec4((light_diffuse + light_ambient) * color, 1.0);
}
