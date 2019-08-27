#ifndef GAME_ENGINE_TEXT_H
#define GAME_ENGINE_TEXT_H

#include "../../../../rendering_engine/shader/shader.hpp"
#include "../../../../rendering_engine/texture.hpp"
#include "meta_file.hpp"
#include "text.hpp"

class font_type;

class text_component : public entity_component
{
public:
    text_component(const std::string& text, const std::string& font_name, float fontSize, vec2f pos, float max_line_length, bool centered) :
            m_meta_data(font_name)
    {
        m_text_string = text;
        m_font_size = fontSize;
        m_max_line_size = max_line_length;
        m_center_text = centered;

        create_text_mesh();
        m_vertex_count = (int) (m_vertices.size() / 2);
        m_tex = new texture("font/" + font_name + ".png");

        glGenVertexArrays(1, &m_vao_id);
        glBindVertexArray(m_vao_id);
        set_attribute(0, 2, m_vertices);
        set_attribute(1, 2, m_tex_coords);
        glBindVertexArray(0);
    }

    void set_attribute(int attributeNumber, int coordinateSize, std::vector<float> data)
    {
        GLuint vboID;
        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
        glVertexAttribPointer(attributeNumber, coordinateSize, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    virtual void init()
    {
        get_shader()->add_uniform("tex");
    }

    virtual void set_all_uni(camera& cam)
    {
    }

    virtual void render() const
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        get_shader()->use_shader();

        m_tex->bind(0);
        get_shader()->set_uniform_1i("tex", 0);

        glBindVertexArray(m_vao_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

private:
    const int m_SPACE_ASCII = 32;

    meta_file m_meta_data;
    std::string m_text_string;
    float m_font_size;

    float m_max_line_size;
    bool m_center_text = false;
    int m_vertex_count;

    texture* m_tex;
    GLuint m_vao_id;

    std::vector<float> m_vertices;
    std::vector<float> m_tex_coords;

    void create_text_mesh();
    std::vector<line> create_structure();
    void complete_structure(std::vector<line> *lines, line current_line, word current_word);
    void create_quad_vert(std::vector<line> lines);
    void add_vertices_for_char(float curser_x, float curser_y, font_char _character, float size_of_font, std::vector<float> *vertices);
    void add_mesh_info(std::vector<float> *vertices, float x, float y, float max_x, float max_y);
};


#endif //GAME_ENGINE_TEXT_H
