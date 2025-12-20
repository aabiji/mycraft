#include <fstream>
#include <sstream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "utils.h"

unsigned int create_shader(const char* path, unsigned int shader_type)
{
    std::ifstream file(path);
    if (!file.good())
        throw Error("Failed to read {}", path);

    std::stringstream file_stream;
    file_stream << file.rdbuf();
    file.close();

    std::string contents = file_stream.str();
    const char* raw_contents = contents.c_str();

    unsigned int shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &raw_contents, nullptr);
    glCompileShader(shader);

    int success;
    char info_log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        throw Error("Failed to create shader: {}", info_log);
    }

    return shader;
}

ShaderProgram::~ShaderProgram()
{
    if (m_program != 0) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

void ShaderProgram::use() { glUseProgram(m_program); }

void ShaderProgram::load(const char* vertex_path, const char* fragment_path)
{
    unsigned int vertex_shader = create_shader(vertex_path, GL_VERTEX_SHADER);
    unsigned int fragment_shader = create_shader(fragment_path, GL_FRAGMENT_SHADER);

    m_program = glCreateProgram();
    glAttachShader(m_program, vertex_shader);
    glAttachShader(m_program, fragment_shader);
    glLinkProgram(m_program);

    int success;
    char info_log[512];
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 512, nullptr, info_log);
        throw Error("Failed to create shader program {}", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

int ShaderProgram::uniform_location(const char* name)
{
    return glGetUniformLocation(m_program, name);
}

template <typename T>
void ShaderProgram::set(const char* name, T& value)
{
    int location = glGetUniformLocation(m_program, name);

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>)
        glUniform1i(location, value);

    else if constexpr (std::is_same_v<T, float>)
        glUniform1f(location, value);

    else if constexpr (std::is_same_v<T, glm::vec3>)
        glUniform3fv(location, 1, glm::value_ptr(value));

    else if constexpr (std::is_same_v<T, glm::vec4>)
        glUniform4fv(location, 1, glm::value_ptr(value));

    else if constexpr (std::is_same_v<T, glm::uvec4>)
        glUniform4uiv(location, 1, glm::value_ptr(value));

    else if constexpr (std::is_same_v<T, glm::mat4>)
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));

    else
        throw Error("Failed to set unknown type");
}

template void ShaderProgram::set<glm::mat4>(const char*, glm::mat4&);
template void ShaderProgram::set<glm::vec3>(const char*, glm::vec3&);
template void ShaderProgram::set<glm::uvec4>(const char*, glm::uvec4&);
template void ShaderProgram::set<float>(const char*, float&);
template void ShaderProgram::set<int>(const char*, int&);
