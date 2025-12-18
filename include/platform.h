#pragma once

#include <utility>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Platform
{
public:
    void open_window(const char* title, float w, float h, bool debug);
    void set_callbacks(bool debug);
    static void shutdown();

    void begin_frame();
    void present_window();
    void toggle_wireframe_mode();

    bool window_close() const;
    void release_mouse() const;
    float aspect_ratio() const;
    bool key_released(int key) const;
    bool key_pressed(int key) const;
    std::pair<float, float> mouse_delta() const;
private:
    struct InputState
    {
        float mouse_x = 0.0;
        float mouse_y = 0.0;
        float mouse_delta_x = 0.0;
        float mouse_delta_y = 0.0;
        bool mouse_moved = false;
        bool key_presses[GLFW_KEY_LAST + 1] = { false };
        bool prev_key_presses[GLFW_KEY_LAST + 1] = { false };
    } m_state;

    GLFWwindow* m_window;
    float m_window_width;
    float m_window_height;
};
