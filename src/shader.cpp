

#include "shader.h"

using namespace std;

Shader::Shader(const string& vertexPath, const string& fragmentPath) {
    std::string vertexCode = Utils::read_shader_file(vertexPath);
    std::string fragmentCode = Utils::read_shader_file(fragmentPath);

    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath,
               const std::string& tessControlPath, const std::string& tessEvalPath) {
    std::string vertexCode = Utils::read_shader_file(vertexPath);
    std::string fragmentCode = Utils::read_shader_file(fragmentPath);
    std::string tessControlCode = Utils::read_shader_file(tessControlPath);
    std::string tessEvalCode = Utils::read_shader_file(tessEvalPath);

    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    const char* tcShaderCode = tessControlCode.c_str();
    const char* teShaderCode = tessEvalCode.c_str();

    // 2. compile shaders
    unsigned int vertex;
    unsigned int fragment;
    unsigned int tessControl;
    unsigned int tessEval;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // Tesselation Control Shader
    tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tessControl, 1, &tcShaderCode, NULL);
    glCompileShader(tessControl);
    checkCompileErrors(tessControl, "TESSELATION CONTROL");
    // Tesselation Evaluation Shader
    tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tessEval, 1, &teShaderCode, NULL);
    glCompileShader(tessEval);
    checkCompileErrors(tessEval, "TESSELATION EVALUATION");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, tessControl);
    glAttachShader(ID, tessEval);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(tessControl);
    glDeleteShader(tessEval);
    glDeleteShader(fragment);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMatrix4(const std::string &name, const glm::mat4 &matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::checkCompileErrors(unsigned int shader, const string& type) const {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

Shader::~Shader() {
     glDeleteProgram(ID);
}




