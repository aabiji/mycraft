#pragma once

#include "camera.h"
#include "platform.h"
#include "shader.h"

class Engine
{
public:
    Engine();
    void run();
private:
    bool m_mouse_enabled;
    Camera m_camera;
    Platform m_platform;
    ShaderProgram m_shader;
};
