#pragma once

#include <utility>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Platform
{
public:
    void open_window(const char* title, float w, float h, bool debug);
    static void shutdown();

    void start_frame();
    void end_frame();

    void set_mouse_mode(bool captured) const;
    void toggle_wireframe_mode();

    bool input_pressed(int code, bool is_mouse) const;
    bool input_released(int code, bool is_mouse) const;

    bool should_close() const;
    float aspect_ratio() const;
    std::pair<float, float> mouse_delta() const;
private:
    void set_callbacks(bool debug);

    struct InputState
    {
        float mouse_x = 0.0;
        float mouse_y = 0.0;
        float mouse_delta_x = 0.0;
        float mouse_delta_y = 0.0;
        bool mouse_moved = false;

        bool mouse_presses[GLFW_MOUSE_BUTTON_LAST + 1] = { false };
        bool prev_mouse_presses[GLFW_MOUSE_BUTTON_LAST + 1] = { false };

        bool key_presses[GLFW_KEY_LAST + 1] = { false };
        bool prev_key_presses[GLFW_KEY_LAST + 1] = { false };
    } m_state;

    GLFWwindow* m_window;
    float m_window_width;
    float m_window_height;
};
