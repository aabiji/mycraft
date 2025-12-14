#include <fstream>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

template <typename...T>
std::string fmt(std::format_string<T...> fmt_str, T&&... args)
{
    return std::format(fmt_str, std::forward<T>(args)...);
}

void gl_debug_output(GLenum source, GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char *message, const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::string source_name = "", error_type = "", severity_msg = "";
    switch (source) {
        case GL_DEBUG_SOURCE_API:             source_name = "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_name = "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_name = "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     source_name = "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     source_name = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           source_name = "Other"; break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               error_type = "Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: error_type = "Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  error_type = "Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         error_type = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         error_type = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              error_type = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          error_type = "Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           error_type = "Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               error_type = "Other"; break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         severity_msg = "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severity_msg = "medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          severity_msg = "low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_msg = "notification"; break;
    }

    std::string msg = fmt("DEBUG: Type: {} | Error type: {} | Severity: {}",
        source_name, error_type, severity_msg);
    std::cout << msg << "\n";

}

void resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void keybinding_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        int mode[2];
        glGetIntegerv(GL_POLYGON_MODE, mode);
        if (mode[0] == GL_LINE)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void process_input(GLFWwindow *window)
{

}

class ShaderProgram
{
public:
    ShaderProgram() : m_program(0) {}
    ~ShaderProgram();

    void use();
    void load(std::string vertex_path, std::string fragment_pat);

    template <typename T>
    void set(std::string& name, T& value);
private:
    unsigned int m_program;
};

unsigned int create_shader(std::string& path, unsigned int shader_type)
{
    std::ifstream file(path);
    if (!file.good())
        throw std::runtime_error(fmt("Failed to read {}", path));

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
        throw std::runtime_error(fmt("Failed to create shader: {}", info_log));
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

void ShaderProgram::load(std::string vertex_path, std::string fragment_path)
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
        throw std::runtime_error(fmt("Failed to create shader program {}", info_log));
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void ShaderProgram::use()
{
    glUseProgram(m_program);
}

template <typename T>
void ShaderProgram::set(std::string& name, T& value)
{
    int location = glGetUniformLocation(m_program, name.c_str());

    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>)
        glUniform1i(location, value);

    else if constexpr (std::is_same_v<T, float>)
        glUniform1f(location, value);

    else if constexpr (std::is_same_v<T, glm::vec3>)
        glUniform3fv(location, 1, glm::value_ptr(value));

    else if constexpr (std::is_same_v<T, glm::vec4>)
        glUniform4fv(location, 1, glm::value_ptr(value));

    else if constexpr (std::is_same_v<T, glm::mat4>)
        glUniformMatrix4fv(location, 1, glm::value_ptr(value));

    else
        throw std::runtime_error("Failed to set unknown type");
}

int main()
{
    if (!glfwInit()) {
        std::cout << "failed to init glfw!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    bool debug = true;
    if (debug)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    int width = 900, height = 700;
    GLFWwindow* window = glfwCreateWindow(width, height, "Mycraft", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(window, resize_callback);
    glfwSetMouseButtonCallback(window, mouse_click_callback);
    glfwSetKeyCallback(window, keybinding_callback);
    glfwSetCursorPosCallback(window, mouse_move_callback);

    glEnable(GL_DEPTH_TEST);

    if (debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    {
        ShaderProgram shader;
        try {
            shader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
        } catch (std::runtime_error& error) {
            std::cout << error.what() << "\n";
            glfwTerminate();
            return -1;
        }

        unsigned int vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        float vertices[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
        };
        unsigned int indices[] = {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        while (!glfwWindowShouldClose(window)) {
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            process_input(window);

            shader.use();
            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    glfwTerminate();
    return 0;
}
