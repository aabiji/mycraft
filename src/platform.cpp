#include <iostream>

#include <glad/glad.h>

#include "platform.h"

Platform::~Platform() { glfwTerminate(); }

bool Platform::window_close() { return glfwWindowShouldClose(m_window); }

float Platform::aspect_ratio() { return m_window_width / m_window_height; }

void Platform::resize_window(float w, float h)
{
    m_window_width = w;
    m_window_height = h;
    glViewport(0, 0, w, h);
}

void Platform::toggle_wireframe_mode()
{
    int mode[2];
    glGetIntegerv(GL_POLYGON_MODE, mode);
    if (mode[0] == GL_LINE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Platform::clear_window()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Platform::present_window()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
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
    if (!m_window)
        throw std::runtime_error("Failed to create window");

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
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

    std::cout << "DEBUG: Type: "   << source_name
              << " | Error type: " << error_type
              << " | Severity: " << severity_msg << "\n";
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

    glfwSetKeyCallback(m_window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            Platform* platform = static_cast<Platform*>(glfwGetWindowUserPointer(window));
            platform->state.key_presses[key] = action != GLFW_RELEASE;
        });

    glfwSetFramebufferSizeCallback(m_window,
        [](GLFWwindow* window, int width, int height)
        {
            Platform* platform = static_cast<Platform*>(glfwGetWindowUserPointer(window));
            platform->resize_window(width, height);
        });

    glfwSetCursorPosCallback(m_window,
        [](GLFWwindow* window, double xpos, double ypos)
        {
            Platform* platform = static_cast<Platform*>(glfwGetWindowUserPointer(window));

            if (platform->state.mouse_moved) {
                platform->state.mouse_delta_x = xpos - platform->state.mouse_x;
                platform->state.mouse_delta_y = ypos - platform->state.mouse_y;
            }

            platform->state.mouse_x = xpos;
            platform->state.mouse_y = ypos;
            platform->state.mouse_moved = true;
        }
    );
}
