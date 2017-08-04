#ifndef ___D_rendering_engine__waterFrameBuffers__
#define ___D_rendering_engine__waterFrameBuffers__

#include <stdio.h>
#include <GL/glew.h>
#include "../../io/window.h"

class water_fbo
{
public:
    water_fbo()
    {
        init_reflect_fbo();
        init_refract_fbo();
    }

    ~water_fbo();

    void bind_reflect_fbo() const { bind_fbo(reflection_fbo, REFLECTION_WIDTH, REFLECTION_HEIGHT); }
    void bind_refract_fbo() const { bind_fbo(refraction_fbo, REFRACTION_WIDTH, REFRACTION_HEIGHT); }


    void unbind_fbo() const;

    inline GLuint get_reflect_tex() const { return reflection_tex; }
    inline GLuint get_refract_tex() const { return refraction_tex; }
    inline GLuint get_refract_depth_tex() const { return refraction_depth_tex; }
    
    void bind_fbo(int frameBuffer, int width, int height) const;
    
    GLuint create_fbo() const;
    GLuint create_tex_attachment(int width, int height) const;
    GLuint create_depth_tex_attachment(int width, int height) const;
    GLuint create_depth_buffer_attachment(int width, int height) const;

private:
    const int REFLECTION_WIDTH = 320;
    const int REFLECTION_HEIGHT = 180;

    const int REFRACTION_WIDTH = 1280;
    const int REFRACTION_HEIGHT = 720;

    GLuint reflection_fbo;
    GLuint reflection_tex;
    GLuint reflection_depth_buffer;

    GLuint refraction_fbo;
    GLuint refraction_tex;
    GLuint refraction_depth_tex;

    void init_reflect_fbo();
    void init_refract_fbo();
};

#endif /* defined(___D_rendering_engine__waterFrameBuffers__) */
