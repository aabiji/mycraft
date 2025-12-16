#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct InputState
{
    InputState()
        : mouse_x(0), mouse_y(0), mouse_delta_x(0),
          mouse_delta_y(0), mouse_moved(false) {};

    float mouse_x;
    float mouse_y;
    float mouse_delta_x;
    float mouse_delta_y;
    bool mouse_moved;
    bool key_presses[255];
};

class Platform
{
public:
    ~Platform();

    void open_window(const char* title, float w, float h, bool debug);
    void set_callbacks(bool debug);

    void resize_window(float w, float h);
    void toggle_wireframe_mode();
    void clear_window();
    void present_window();

    bool window_close();
    float aspect_ratio();
    InputState get_input_state();

    InputState state;
private:
    GLFWwindow* m_window;
    float m_window_width;
    float m_window_height;
};
