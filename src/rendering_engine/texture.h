#ifndef GAME_ENGINE_TEXTURE_H
#define GAME_ENGINE_TEXTURE_H

#include <GL/glew.h>
#include <string>
#include <map>

class texture_data
{
public:
    texture_data(GLenum textureTarget, int width, int height, int numTextures, unsigned char** data, GLfloat* filters, GLenum* internalFormat, GLenum* format, bool clamp, GLenum* attachments);
	texture_data(int width, int height);

	void bind(int textureNum, bool bind_manu) const;
    void bind_as_render_target() const;
    void unbind_render_target(int width, int height) const;
	
	inline int get_width()  const { return m_width; }
	inline int get_height() const { return m_height; }
	
	virtual ~texture_data();
protected:	
private:
	texture_data(texture_data& other) {}
	void operator=(texture_data& other) {}

	void init_textures(unsigned char **data, GLfloat *filter, GLenum *internalFormat, GLenum *format, bool clamp);
	void init_render_targets(GLenum *attachments);


	GLuint* m_textureID;
	GLenum m_textureTarget;
	GLuint m_frameBuffer;
	GLuint m_renderBuffer;
	int m_numTextures;
	int m_width;
	int m_height;
};



class texture
{
public:
	texture(const std::string& file_name, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_LINEAR_MIPMAP_LINEAR, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, bool clamp = false, GLenum attachment = GL_NONE);
	texture(int width = 0, int height = 0, unsigned char* data = 0, GLenum textureTarget = GL_TEXTURE_2D, GLfloat filter = GL_LINEAR_MIPMAP_LINEAR, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, bool clamp = false, GLenum attachment = GL_NONE);
    texture(int width, int height, bool is_buffer)
    {
        m_fileName = "";
        m_textureData = new texture_data(width, height);
    }

	texture(const texture& tex);
	virtual ~texture();
    
    texture sub_image(int x, int y, int width, int height);
	void bind(unsigned int unit = 0, bool bind_manu = false) const;
	void bind_as_render_target() const;
    void unbind_render_target(int width, int height) const;


    inline int get_width()  const { return m_textureData->get_width(); }
	inline int get_height() const { return m_textureData->get_height(); }
	
	bool operator==(const texture& tex) const { return m_textureData == tex.m_textureData; }
	bool operator!=(const texture& tex) const { return !operator==(tex); }
protected:
private:
	static std::map<std::string, texture_data*> s_resourceMap;

	texture_data* m_textureData;
	std::string m_fileName;
};

#endif