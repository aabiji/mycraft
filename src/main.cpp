#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "shader.h"
#include "window.h"

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

int main()
{
    try {
        Camera camera;
        ShaderProgram shader;
        InputState state;
        Window window;

        window.open("Mycraft", 900, 700, true);
        window.set_callbacks(state);

        shader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

        while (!window.quit()) {
            window.clear();

            if (state.key_presses[GLFW_KEY_W])     camera.move(Direction::front);
            if (state.key_presses[GLFW_KEY_S])     camera.move(Direction::back);
            if (state.key_presses[GLFW_KEY_A])     camera.move(Direction::left);
            if (state.key_presses[GLFW_KEY_D])     camera.move(Direction::right);
            if (state.key_presses[GLFW_KEY_UP])    camera.move(Direction::up);
            if (state.key_presses[GLFW_KEY_DOWN])  camera.move(Direction::down);
            if (state.key_presses[GLFW_KEY_SPACE]) window.toggle_wireframe_mode();

            // TODO: rotate mouse
            // TODO: does toggling wireframe work properly?

            glm::mat4 model = glm::mat4(1.0);
            glm::mat4 projection =
                glm::perspective(glm::radians(45.0f), window.aspect_ratio(), 0.1f, 100.0f);
            glm::mat4 view = camera.view_matrix();

            shader.use();
            shader.set<glm::mat4>("model", model);
            shader.set<glm::mat4>("view", view);
            shader.set<glm::mat4>("projection", projection);

            window.present();
        }
    } catch (std::runtime_error& error) {
        std::cout << error.what() << "\n";
    }

    return 0;
}
