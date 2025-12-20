#include <assert.h>
#include <glad/glad.h>

#include "idbuffer.h"

IDBuffer::~IDBuffer()
{
    glDeleteFramebuffers(1, &m_framebuffer);
    glDeleteRenderbuffers(1, &m_depth_buffer);
    glDeleteTextures(1, &m_texture);
}

void IDBuffer::enable()
{
    unsigned int clearValue = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClearBufferuiv(GL_COLOR, 0, &clearValue);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void IDBuffer::disable() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

unsigned int IDBuffer::sample(int x, int y)
{
    unsigned int pixel = 0;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebuffer);
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);
    return pixel;
}

void IDBuffer::use_texture(int texture_location)
{
    glActiveTexture(GL_TEXTURE0 + m_texture_index);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glUniform1i(texture_location, m_texture_index);
}

void IDBuffer::create(int width, int height, int texture_index)
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // create and attach the texture to the framebuffer
    m_texture_index = texture_index;
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width, height); // 1 pixel = 1 u32
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

    // create and attach the depth buffer
    glGenRenderbuffers(1, &m_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_buffer);

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void IDBuffer::resize(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0,
                 GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
