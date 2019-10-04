#ifndef INC_3D_ENGINE_FBO_HPP
#define INC_3D_ENGINE_FBO_HPP


#include <GL/glew.h>
#include "../core_engine/core_engine.hpp"

extern core_engine *g_core_engine;


class fbo {

    int NONE = 0;
    int DEPTH_TEXTURE = 1;
    int DEPTH_RENDER_BUFFER = 2;

    int width;
    int height;

    GLuint frame_buffer;

    GLuint color_texture;
    GLuint depth_texture;

    GLuint depth_buffer;
    GLuint color_buffer;
    
    fbo(int width, int height, int depth_buffer_type) {
        this->width = width;
        this->height = height;
        initialise_frame_buffer(depth_buffer_type);
    }

    /**
     * Deletes the frame buffer and its attachments when the game closes.
     */
    ~fbo() {
        glDeleteFramebuffers(1, &frame_buffer);
        glDeleteTextures(1, &color_texture);
        glDeleteTextures(1, &depth_texture);
        glDeleteRenderbuffers(1, &depth_buffer);
        glDeleteRenderbuffers(1, &color_buffer);
    }

    /**
     * Binds the frame buffer, setting it as the current render target. Anything
     * rendered after this will be rendered to this FBO, and not to the screen.
     */
    void bind_frame_buffer() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
        glViewport(0, 0, width, height);
    }
    /**
     * Unbinds the frame buffer, setting the default frame buffer as the current
     * render target. Anything rendered after this will be rendered to the
     * screen, and not this FBO.
     */
    void unbind_frame_buffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, g_core_engine->get_window()->get_width(), g_core_engine->get_window()->get_height());
    }

    /**
     * Binds the current FBO to be read from (not used in tutorial 43).
     */
    void bind_to_read() {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
    }

    /**
     * @return The ID of the texture containing the colour buffer of the FBO.
     */
    int get_color_texture() {
        return color_texture;
    }

    /**
     * @return The texture containing the FBOs depth buffer.
     */
    int get_depth_texture() {
        return depth_texture;
    }

    /**
     * Creates the FBO along with a colour buffer texture attachment, and
     * possibly a depth buffer.
     * 
     * @param type
     *            - the type of depth buffer attachment to be attached to the
     *            FBO.
     */
    void initialise_frame_buffer(int type) {
        create_frame_buffer();
        create_texture_attachment();
        if (type == DEPTH_RENDER_BUFFER) {
            create_depth_buffer_attachment();
        } else if (type == DEPTH_TEXTURE) {
            create_depth_texture_attachment();
        }
        unbind_frame_buffer();
    }

    /**
     * Creates a new frame buffer object and sets the buffer to which drawing
     * will occur - colour attachment 0. This is the attachment where the colour
     * buffer texture is.
     * 
     */
    void create_frame_buffer() {
        glGenFramebuffers(1, &frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    /**
     * Creates a texture and sets it as the colour buffer attachment for this
     * FBO.
     */
    void create_texture_attachment() {
        glGenTextures(1, &color_texture);
        glBindTexture(GL_TEXTURE_2D, color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    }

    /**
     * Adds a depth buffer to the FBO in the form of a texture, which can later
     * be sampled.
     */
    void create_depth_texture_attachment() {
        glGenTextures(1, &depth_texture);
        glBindTexture(GL_TEXTURE_2D, depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    }

    /**
     * Adds a depth buffer to the FBO in the form of a render buffer. This can't
     * be used for sampling in the shaders.
     */
    void create_depth_buffer_attachment() {
        glGenRenderbuffers(1, &depth_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
    }
};


#endif //INC_3D_ENGINE_FBO_HPP
