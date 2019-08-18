#ifndef ___D_rendering_engine__waterFrameBuffers__
#define ___D_rendering_engine__waterFrameBuffers__

#include <stdio.h>
#include <GL/glew.h>

class water_fbo
{
public:
    water_fbo()
    {
        init_reflect_fbo();
        init_refract_fbo();
    }

    ~water_fbo();

    void bind_reflect_fbo() const { bind_fbo(m_reflection_fbo, m_REFLECTION_WIDTH, m_REFLECTION_HEIGHT); }
    void bind_refract_fbo() const { bind_fbo(m_refraction_fbo, m_REFRACTION_WIDTH, m_REFRACTION_HEIGHT); }


    void unbind_fbo() const;

    inline GLuint get_reflect_tex() const { return m_reflection_tex; }
    inline GLuint get_refract_tex() const { return m_refraction_tex; }
    inline GLuint get_refract_depth_tex() const { return m_refraction_depth_tex; }
    
    void bind_fbo(int frameBuffer, int width, int height) const;
    
    GLuint create_fbo() const;
    GLuint create_tex_attachment(int width, int height) const;
    GLuint create_depth_tex_attachment(int width, int height) const;
    GLuint create_depth_buffer_attachment(int width, int height) const;

private:
    const int m_REFLECTION_WIDTH = 320;
    const int m_REFLECTION_HEIGHT = 180;

    const int m_REFRACTION_WIDTH = 1280;
    const int m_REFRACTION_HEIGHT = 720;

    GLuint m_reflection_fbo;
    GLuint m_reflection_tex;
    GLuint m_reflection_depth_buffer;

    GLuint m_refraction_fbo;
    GLuint m_refraction_tex;
    GLuint m_refraction_depth_tex;

    void init_reflect_fbo();
    void init_refract_fbo();
};

#endif /* defined(___D_rendering_engine__waterFrameBuffers__) */
