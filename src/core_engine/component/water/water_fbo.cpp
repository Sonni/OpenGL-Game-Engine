#include "water_fbo.h"
#include "../../io/window.hpp"

water_fbo::~water_fbo()
{
    glDeleteFramebuffers(1, &m_reflection_fbo);
    glDeleteTextures(1, &m_reflection_tex);
    glDeleteRenderbuffers(1, &m_reflection_depth_buffer);
    glDeleteFramebuffers(1, &m_refraction_fbo);
    glDeleteTextures(1, &m_refraction_tex);
    glDeleteTextures(1, &m_refraction_depth_tex);
}

void water_fbo::unbind_fbo() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window::WIN_WIDTH, window::WIN_HEIGHT);
}

void water_fbo::bind_fbo(int frameBuffer, int width, int height) const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

GLuint water_fbo::create_fbo() const
{
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return frameBuffer;
}

GLuint water_fbo::create_tex_attachment(int width, int height) const
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
    return texture;
}

GLuint water_fbo::create_depth_tex_attachment(int width, int height) const
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
    return texture;
}

GLuint water_fbo::create_depth_buffer_attachment(int width, int height) const
{
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}

void water_fbo::init_reflect_fbo()
{
    m_reflection_fbo = create_fbo();
    m_reflection_tex = create_tex_attachment(m_REFLECTION_WIDTH, m_REFLECTION_HEIGHT);
    m_reflection_depth_buffer = create_depth_buffer_attachment(m_REFLECTION_WIDTH, m_REFLECTION_HEIGHT);
    unbind_fbo();
}

void water_fbo::init_refract_fbo()
{
    m_refraction_fbo = create_fbo();
    m_refraction_tex = create_tex_attachment(m_REFRACTION_WIDTH, m_REFRACTION_HEIGHT);
    m_refraction_depth_tex = create_depth_tex_attachment(m_REFRACTION_WIDTH, m_REFRACTION_HEIGHT);
    unbind_fbo();
}