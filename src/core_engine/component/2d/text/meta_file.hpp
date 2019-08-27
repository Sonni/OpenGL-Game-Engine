#ifndef GAME_ENGINE_META_FILE_H
#define GAME_ENGINE_META_FILE_H

#include <vector>
#include <map>
#include <fstream>

struct font_char
{
public:
    font_char(int ascii, float x_tex_coord, float y_tex_coord, float x_tex_size, float y_tex_size, float x_offset, float y_offset, float x_size, float y_size, float x_advance)
    {
        m_ascii = ascii;
        m_x_tex_coord = x_tex_coord;
        m_y_tex_coord = y_tex_coord;
        m_x_offset = x_offset;
        m_y_offset = y_offset;
        m_x_size = x_size;
        m_y_size = y_size;
        m_x_max_tex_coord = x_tex_size + x_tex_coord;
        m_y_max_tex_coord = y_tex_size + y_tex_coord;
        m_x_advance = x_advance;
    }

    int get_ascii() const { return m_ascii; }

    float get_x_tex_coord() const { return m_x_tex_coord; }
    float get_y_tex_coord() const { return m_y_tex_coord; }

    float get_x_max_tex_coord() const { return m_x_max_tex_coord; }
    float get_y_max_tex_coord() const { return m_y_max_tex_coord; }

    float get_x_offset() const { return m_x_offset; }
    float get_y_offset() const { return m_y_offset; }

    float get_x_size() const { return m_x_size; }
    float get_y_size() const { return m_y_size; }

    float get_x_advance() const { return m_x_advance; }

private:
    int m_ascii;
    float m_x_tex_coord, m_y_tex_coord;
    float m_x_max_tex_coord, m_y_max_tex_coord;
    float m_x_offset, m_y_offset;
    float m_x_size, m_y_size;
    float m_x_advance;
};

class meta_file
{
public:
    meta_file(const std::string& font_name);

    float getSpaceWidth() const { return m_space_width; }
    font_char* getCharacter(int ascii) { return m_meta_data[ascii]; }


private:
    const int m_PAD_TOP = 0;
    const int m_PAD_LEFT = 1;
    const int m_PAD_BOTTOM = 2;
    const int m_PAD_RIGHT = 3;
    const int m_ASCII_SPACE = 32;

    const int m_DESIRED_PADDING = 3;

    float m_ratio;

    float m_verticalPerPixelSize;
    float m_horizontalPerPixelSize;
    float m_space_width;
    std::vector<int> m_padding;
    int m_padding_width;
    int m_padding_height;

    std::map<int, font_char*> m_meta_data;
    std::map<std::string, std::string> m_values;

    std::ifstream m_file;

    bool process_next_line();

    int get_value_of_var(std::string variable);
    std::vector<int> get_values_of_var(std::string variable);

    void load_padding();
    void load_line_size();
    void load_char_data(int image_width);

    font_char* load_char(int image_size);
};


#endif //GAME_ENGINE_META_FILE_H