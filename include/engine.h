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
    Camera m_camera;
    bool m_mouse_enabled;
    Platform m_platform;
    ShaderProgram m_shader;
};
