#include <algorithm>
#include <iostream>

#include <glad/glad.h>

#include "platform.h"

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

    std::cout
        << source_name << " " << error_type  << " | "
        << severity_msg << " severity | " << message << "\n";
}

void Platform::open_window(const char* title, float width, float height, bool debug)
{
    m_window_width = width;
    m_window_height = height;

    if (!glfwInit())
        throw std::runtime_error("Failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    set_callbacks(debug);
}

void Platform::set_callbacks(bool debug)
{
    glfwSetWindowUserPointer(m_window, this);

    if (debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwSetFramebufferSizeCallback(m_window,
        [](GLFWwindow* window, int width, int height)
        {
            Platform* platform = static_cast<Platform*>(glfwGetWindowUserPointer(window));
            platform->m_window_width = width;
            platform->m_window_height = height;
            glViewport(0, 0, width, height);
        });

    glfwSetCursorPosCallback(m_window,
        [](GLFWwindow* window, double xpos, double ypos)
        {
            Platform* platform = static_cast<Platform*>(glfwGetWindowUserPointer(window));
            bool mouse_captured = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

            if (platform->m_state.mouse_moved && mouse_captured) {
                platform->m_state.mouse_delta_x = xpos - platform->m_state.mouse_x;
                // since opengl defines (0, 0) to be the bottom left corner
                platform->m_state.mouse_delta_y = platform->m_state.mouse_y - ypos;
            }

            platform->m_state.mouse_x = xpos;
            platform->m_state.mouse_y = ypos;
            platform->m_state.mouse_moved = true;
        }
    );
}

void Platform::shutdown() { glfwTerminate(); }

void Platform::set_mouse_mode(bool captured) const
{
    glfwSetInputMode(m_window, GLFW_CURSOR,
        captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Platform::toggle_wireframe_mode()
{
    int mode[2];
    glGetIntegerv(GL_POLYGON_MODE, mode);
    glPolygonMode(GL_FRONT_AND_BACK, mode[0] == GL_LINE ? GL_FILL : GL_LINE);
}

bool Platform::should_close() const { return glfwWindowShouldClose(m_window); }

float Platform::aspect_ratio() const { return m_window_width / m_window_height; }

bool Platform::input_pressed(int code, bool is_mouse) const
{
    if (is_mouse)
        return m_state.mouse_presses[code];
    return m_state.key_presses[code];
}

bool Platform::input_released(int code, bool is_mouse) const
{
    if (is_mouse)
        return !m_state.mouse_presses[code] && m_state.prev_mouse_presses[code];
    return !m_state.key_presses[code] && m_state.prev_key_presses[code];
}

std::pair<float, float> Platform::mouse_delta() const
{
    return { m_state.mouse_delta_x, m_state.mouse_delta_y };
}

void Platform::start_frame()
{
    glClearColor(0.61, 0.85, 0.95, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // poll input
    std::copy(
        std::begin(m_state.key_presses),
        std::end(m_state.key_presses),
        std::begin(m_state.prev_key_presses));
    for (int key = 0; key < GLFW_KEY_LAST; key++)
        m_state.key_presses[key] = glfwGetKey(m_window, key) == GLFW_PRESS;

    std::copy(
        std::begin(m_state.mouse_presses),
        std::end(m_state.mouse_presses),
        std::begin(m_state.prev_mouse_presses));
    for (int btn = 0; btn < GLFW_MOUSE_BUTTON_LAST; btn++)
        m_state.mouse_presses[btn] = glfwGetMouseButton(m_window, btn) == GLFW_PRESS;
}

void Platform::end_frame()
{
    m_state.mouse_delta_x = 0;
    m_state.mouse_delta_y = 0;

    glfwPollEvents();
    glfwSwapBuffers(m_window);
}
