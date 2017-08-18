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
        this->ascii = ascii;
        this->x_tex_coord = x_tex_coord;
        this->y_tex_coord = y_tex_coord;
        this->x_offset = x_offset;
        this->y_offset = y_offset;
        this->x_size = x_size;
        this->y_size = y_size;
        this->x_max_tex_coord = x_tex_size + x_tex_coord;
        this->y_max_tex_coord = y_tex_size + y_tex_coord;
        this->x_advance = x_advance;
    }

    int get_ascii() const { return ascii; }

    float get_x_tex_coord() const { return x_tex_coord; }
    float get_y_tex_coord() const { return y_tex_coord; }

    float get_x_max_tex_coord() const { return x_max_tex_coord; }
    float get_y_max_tex_coord() const { return y_max_tex_coord; }

    float get_x_offset() const { return x_offset; }
    float get_y_offset() const { return y_offset; }

    float get_x_size() const { return x_size; }
    float get_y_size() const { return y_size; }

    float get_x_advance() const { return x_advance; }

private:
    int ascii;
    float x_tex_coord, y_tex_coord;
    float x_max_tex_coord, y_max_tex_coord;
    float x_offset, y_offset;
    float x_size, y_size;
    float x_advance;
};

class meta_file
{
public:
    meta_file(const std::string& font_name);

    float getSpaceWidth() const { return space_width; }
    font_char* getCharacter(int ascii) { return meta_data[ascii]; }


private:
    const int PAD_TOP = 0;
    const int PAD_LEFT = 1;
    const int PAD_BOTTOM = 2;
    const int PAD_RIGHT = 3;
    const int ASCII_SPACE = 32;

    const int DESIRED_PADDING = 3;

    float ratio;

    float verticalPerPixelSize;
    float horizontalPerPixelSize;
    float space_width;
    std::vector<int> padding;
    int padding_width;
    int padding_height;

    std::map<int, font_char*> meta_data;
    std::map<std::string, std::string> values;

    std::ifstream file;

    bool process_next_line();

    int get_value_of_var(std::string variable);
    std::vector<int> get_values_of_var(std::string variable);

    void load_padding();
    void load_line_size();
    void load_char_data(int image_width);

    font_char* load_char(int image_size);
};


#endif //GAME_ENGINE_META_FILE_H