#pragma once

class ShaderProgram
{
public:
    ShaderProgram() : m_program(0) {}
    ~ShaderProgram();

    void use();
    void load(const char* vertex_path, const char* fragment_pat);
    int uniform_location(const char* name);

    template <typename T>
    void set(const char* name, T& value);
private:
    unsigned int m_program;
};
