#pragma once

#include "camera.h"
#include "idbuffer.h"
#include "platform.h"
#include "shader.h"

class Engine
{
public:
    Engine();
    void run();
private:
    Camera m_camera;
    bool m_mouse_enabled;

    IDBuffer m_id_buffer;
    ShaderProgram m_id_buffer_shader;

    Platform m_platform;
    ShaderProgram m_main_shader;
};
