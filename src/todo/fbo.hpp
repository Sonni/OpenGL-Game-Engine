#ifndef INC_3D_ENGINE_FBO_HPP
#define INC_3D_ENGINE_FBO_HPP

#include <GL/glew.h>
#include "../core_engine/io/window.hpp"


class fbo {
public:
    int m_NONE = 0;
    int m_DEPTH_TEXTURE = 1;
    int m_DEPTH_RENDER_BUFFER = 2;

    int m_width;
    int m_height;

    GLuint m_frame_buffer;

    GLuint m_color_texture;
    GLuint m_depth_texture;

    GLuint m_depth_buffer;
    GLuint m_color_buffer;

    window* m_window;

    /**
     * Creates an FBO of a specified width and height, with the desired type of
     * depth buffer attachment.
     *
     * @param width
     *            - the width of the FBO.
     * @param height
     *            - the height of the FBO.
     * @param depthBufferType
     *            - an int indicating the type of depth buffer attachment that
     *            this FBO should use.
     */
    fbo(int width, int height, window *Window, int depthBufferType) {
        m_width = width;
        m_height = height;
        m_window = Window;
        initialiseFrameBuffer(depthBufferType);
    }

    /**
     * Deletes the frame buffer and its attachments when the game closes.
     */
    void cleanUp() {
        glDeleteFramebuffers(1, &m_frame_buffer);
        glDeleteTextures(1, &m_color_texture);
        glDeleteTextures(1, &m_depth_texture);
        glDeleteRenderbuffers(1, &m_depth_buffer);
        glDeleteRenderbuffers(1, &m_color_buffer);
    }

    /**
     * Binds the frame buffer, setting it as the current render target. Anything
     * rendered after this will be rendered to this FBO, and not to the screen.
     */
    void bindFrameBuffer() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frame_buffer);
        glViewport(0, 0, m_width, m_height);
    }

    /**
     * Unbinds the frame buffer, setting the default frame buffer as the current
     * render target. Anything rendered after this will be rendered to the
     * screen, and not this FBO.
     */
    void unbindFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_window->get_width(), m_window->get_height());
    }

    /**
     * Binds the current FBO to be read from (not used in tutorial 43).
     */
    void bindToRead() {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_frame_buffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }

    /**
     * @return The ID of the texture containing the colour buffer of the FBO.
     */
    int getColourTexture() {
        return m_color_texture;
    }

    /**
     * @return The texture containing the FBOs depth buffer.
     */
    int getDepthTexture() {
        return m_depth_texture;
    }

    /**
     * Creates the FBO along with a colour buffer texture attachment, and
     * possibly a depth buffer.
     *
     * @param type
     *            - the type of depth buffer attachment to be attached to the
     *            FBO.
     */
    void initialiseFrameBuffer(int type) {
        createFrameBuffer();
        createTextureAttachment();
        if (type == m_DEPTH_RENDER_BUFFER) {
            createDepthBufferAttachment();
        } else if (type == m_DEPTH_TEXTURE) {
            createDepthTextureAttachment();
        }
        unbindFrameBuffer();
    }

    /**
     * Creates a new frame buffer object and sets the buffer to which drawing
     * will occur - colour attachment 0. This is the attachment where the colour
     * buffer texture is.
     *
     */
    void createFrameBuffer() {
        glGenFramebuffers(1, &m_frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    /**
     * Creates a texture and sets it as the colour buffer attachment for this
     * FBO.
     */
    void createTextureAttachment() {
        glGenTextures(1, &m_color_texture);
        glBindTexture(GL_TEXTURE_2D, m_color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);
    }

    /**
     * Adds a depth buffer to the FBO in the form of a texture, which can later
     * be sampled.
     */
    void createDepthTextureAttachment() {
        glGenTextures(1, &m_depth_texture);
        glBindTexture(GL_TEXTURE_2D, m_depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);
    }

    /**
     * Adds a depth buffer to the FBO in the form of a render buffer. This can't
     * be used for sampling in the shaders.
     */
    void createDepthBufferAttachment() {
        glGenRenderbuffers(1, &m_depth_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_buffer);
    }

private:
};


#endif //INC_3D_ENGINE_FBO_HPP
