#pragma once

// Stores rendered object ids to a framebuffer for post processing edge detection
class IDBuffer
{
public:
    ~IDBuffer();
    void enable();
    void disable();
    void create(int width, int height);
    void resize(int width, int height);
    unsigned int sample(int x, int y);
private:
    unsigned int m_framebuffer;
    unsigned int m_depth_buffer;
    unsigned int m_texture;
};