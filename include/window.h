#pragma once

// TODO: initialize!
struct InputState
{
    float mouse_x;
    float mouse_y;
    float mouse_delta_x;
    float mouse_delta_y;
    bool first_mouse_move;
    bool key_presses[255];
};

class Window
{
public:
    ~Window();

    void open(const char* title, float width, float height, bool debug);
    void set_callbacks(InputState& input_state);
    void resize(float w, float h);
    void toggle_wireframe_mode();

    float aspect_ratio();
    bool quit();

    void clear();
    void present();
private:
    GLFWwindow* m_window;
    float m_window_width;
    float m_window_height;
};
