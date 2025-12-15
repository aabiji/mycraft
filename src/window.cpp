#include "window.h"

Window::~Window() { glfwTerminate(); }

void Window::open(const char* title, float width, float height, bool debug)
{
    m_window_width = width;
    m_window_height = height;

    if (!glfwInit())
        throw std::runtime_error("Failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug);

    m_window =  glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window)
        throw std::runtime_error("Failed to create window");

    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    if (debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

void Window::set_callbacks(InputState& state)
{
    glfwSetWindowUserPointer(m_window, &state);

    glfwSetKeyCallback(m_window,
        [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            InputState* state = static_cast<InputState*>(glfwGetWindowUserPointer(window));
            state->key_presses[key] = action != GLFW_RELEASE;
        }]);

    glfwSetFramebufferSizeCallback(m_window,
        [](GLFWwindow* window, int width, int height)
        {
            // TODO: problem!
            Window* window_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
            window_ptr->resize(width, height);
        }]);

    glfwSetCursorPosCallback(m_window,
        [](GLFWwindow* window, double xpos, double ypos)
        {
            InputState* state = static_cast<InputState*>(glfwGetWindowUserPointer(window));

            if (!state->first_mouse_move) {
                state->mouse_delta_x = xpos - state->mouse_x;
                state->mouse_delta_y = ypos - state->mouse_y;
            }

            state->mouse_x = xpos;
            state->mouse_y = ypos;
            state->first_mouse_move = false;
        }
    );
}

void Window::resize(float w, float h)
{
    m_window_width = w;
    m_window_height = h;
    glViewport(0, 0, w, h);
}

bool Window::quit() { glfwWindowShouldClose(m_window); }

float Window::aspect_ratio() { return m_window_width / m_window_height; }

void Window::toggle_wireframe_mode()
{
    int mode[2];
    glGetIntegerv(GL_POLYGON_MODE, mode);
    if (mode[0] == GL_LINE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Window::clear()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::present()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}
