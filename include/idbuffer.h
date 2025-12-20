#pragma once

#include <glm/glm.hpp>

// Stores rendered object ids to a framebuffer for post processing edge detection
class IDBuffer
{
public:
    ~IDBuffer();

    void enable();
    void disable();

    void resize(int width, int height);
    void create(int width, int height, int texture_index);

    void use_texture(int texture_location);
    glm::uvec4 sample(int x, int y);
private:
    unsigned int m_framebuffer;
    unsigned int m_depth_buffer;
    unsigned int m_texture;
    int m_texture_index;
};