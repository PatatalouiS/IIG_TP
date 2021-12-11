#ifndef SHADER_H
#define SHADER_H

#include "opengl_stuff.h"
#include "utils.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

class Shader
{
    public:
        unsigned int ID;

        // Vertex and fragment shaders
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        // Vertex, tessellation control, tessellation eval and fragment shaders
        Shader(const std::string& vertexPath, const std::string& fragmentPath,
               const std::string& tessControlPath, const std::string& tessEvalPath);

        // activate the shader
        void use() const;

        // set Boolean Uniform
        void setBool(const std::string &name, bool value) const;

        // set Integer Uniform
        void setInt(const std::string &name, int value) const;

        // set Float Uniform
        void setFloat(const std::string &name, float value) const;

        // set Matrix4
        void setMatrix4(const std::string& name, const glm::mat4& matrix) const;

        // set vec3
        void setVec3(const std::string& name, const glm::vec3& vec) const;

        ~Shader();

    private:

        // utility function for checking shader compilation/linking errors.
        void checkCompileErrors(unsigned int shader, const std::string& type) const;
};
#endif
