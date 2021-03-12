#include "shaders.h"

const char* Shaders::vertex_shader_normals = "#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        layout (location = 1) in vec3 inormal;\n\
        uniform mat4 model;\n\
        uniform mat4 view;\n\
        uniform mat4 projection;\n\
        out vec3 normal;\n\
        void main()\n\
        {\n\
            gl_Position = projection * view * model * vec4(position, 1.0f);\n\
            normal = inormal;\n\
        }\n";

const char* Shaders::fragment_shader_normals = "#version 410 core\n\
        in vec3 normal;\n\
        out vec4 color;\n\
        void main()\n\
        {\n\
            color = vec4(normalize(normal)*0.5+0.5, 1);\n\
        }\n";

const char* Shaders::vertex_shader_error = "#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        uniform mat4 model;\n\
        uniform mat4 view;\n\
        uniform mat4 projection;\n\
        out vec3 glpos;\n\
        void main()\n\
        {\n\
            gl_Position = projection * view * model * vec4(position, 1.0f);\n\
            glpos = position;\n\
        }\n";

const char* Shaders::fragment_shader_error = "#version 410 core\n\
        in vec3 glpos;\n\
        out vec3 color;\n\
        void main()\n\
        {\n\
            float error = (1 - length(glpos)) * 30;\n\
            color = vec3(error, error, error);\n\
        }\n";

const char* Shaders::vertex_shader_lighting = "#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        layout (location = 1) in vec3 inormal;\n\
        uniform mat4 model;\n\
        uniform mat4 view;\n\
        uniform mat4 projection;\n\
        out vec3 glpos;\n\
        out vec3 normal;\n\
        out vec3 fragment_position;\n\
        void main()\n\
        {\n\
            gl_Position = projection * view * model * vec4(position, 1.0f);\n\
            fragment_position = vec3(model * vec4(position, 1.0));\n\
            glpos = position;\n\
            normal = mat3(transpose(inverse(model))) * inormal;\n\
        }\n";

const char* Shaders::fragment_shader_lighting = "#version 410 core\n\
        in vec3 normal;\n\
        in vec3 glpos;\n\
        in vec3 fragment_position;\n\
        out vec4 general_color;\n\
        uniform vec3 light_position = vec3(3, 0, 3);\n\
        uniform vec3 light_color = vec3(1,1,1);\n\
        void main()\n\
        {\n\
            vec3 light_ambient = 0.25 * light_color;\n\
            vec3 norm_ = normalize(normal);\n\
            vec3 light_direction = normalize(light_position - fragment_position);\n\
            vec3 light_diffuse = max(dot(norm_, light_direction), 0.0) * light_color;\n\
            vec3 color = vec3(normalize(normal)*0.5+0.5);\n\
            general_color = vec4((light_diffuse + light_ambient) * color, 1.0);\n\
        }\n";

const char* Shaders::vertex_shader_3lighting = "#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        layout (location = 1) in vec3 inormal;\n\
        uniform mat4 model;\n\
        uniform mat4 view;\n\
        uniform mat4 projection;\n\
        out vec3 glpos;\n\
        out vec3 normal;\n\
        out vec3 fragment_position;\n\
        void main()\n\
        {\n\
            gl_Position = projection * view * model * vec4(position, 1.0f);\n\
            fragment_position = vec3(model * vec4(position, 1.0));\n\
            glpos = position;\n\
            normal =  mat3(transpose(inverse(model))) * inormal;\n\
        }\n";

const char* Shaders::fragment_shader_3lighting = "#version 410 core\n\
        in vec3 normal;\n\
        in vec3 glpos;\n\
        in vec3 fragment_position;\n\
        out vec4 general_color;\n\
        uniform vec3 pos_key_light = vec3(-40, 175, 100);\n\
        uniform vec3 pos_fill_light = vec3(70, -25, 100);\n\
        uniform vec3 pos_back_light = vec3(20, 0, -20);\n\
        uniform vec3 light_color = vec3(1,0.92,0.87);\n\
        void main()\n\
        {\n\
            vec3 light_ambient = 0.075 * light_color;\n\
            vec3 norm_ = normalize(normal);\n\
            vec3 dir_key_light = normalize(pos_key_light - fragment_position);\n\
            vec3 dir_fill_light = normalize(pos_fill_light - fragment_position);\n\
            vec3 dir_back_light = normalize(pos_back_light - fragment_position);\n\
            vec3 key_light = max(dot(norm_, dir_key_light), 0.0) * light_color * 1.75;\n\
            vec3 fill_light = max(dot(norm_, dir_fill_light), 0.0) * light_color * 0.5;\n\
            vec3 back_light = max(dot(norm_, dir_back_light), 0.0) * light_color * 2;\n\
            vec3 color = vec3(0.8,0,0);\n\
            general_color = vec4((key_light + fill_light + back_light) * color * 0.8, 1.0);\n\
        }\n";

