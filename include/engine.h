#pragma once

#include "camera.h"
#include "platform.h"
#include "shader.h"
#include "texture.h"
#include "terrain.h"

class Engine
{
public:
    Engine();
    void run();
private:
    void handle_input();
    void set_shader_state();

    bool m_mouse_enabled;
    RaycastHit m_hit;
    Camera m_camera;
    Terrain m_terrain;
    Platform m_platform;
    TextureAtlas m_texture_atlas;
    ShaderProgram m_shader;
};
