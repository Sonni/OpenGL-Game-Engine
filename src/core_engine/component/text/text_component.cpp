#include "text_component.h"

void text_component::create_text_mesh()
{
    std::vector<line> lines = create_structure();
    create_quad_vert(lines);
}

std::vector<line> text_component::create_structure()
{
    std::string chars = text_string;
    std::vector<line> lines;
    line current_line(meta_data.getSpaceWidth(), font_size, max_line_size);
    word current_word(font_size);
    for (char ascii : chars)
    {
        if (ascii == SPACE_ASCII)
        {
            bool added = current_line.add_word(current_word);
            if (!added)
            {
                lines.push_back(current_line);
                line curLine(meta_data.getSpaceWidth(), font_size, max_line_size);
                current_line = curLine;
                current_line.add_word(current_word);
            }
            word curWord(font_size);
            current_word = curWord;
            continue;
        }
        font_char* _character = meta_data.getCharacter(ascii);
        current_word.add_char(_character);
    }
    complete_structure(&lines, current_line, current_word);
    return lines;
}

void text_component::complete_structure(std::vector<line> *lines, line current_line, word current_word)
{
    bool added = current_line.add_word(current_word);
    if (!added)
    {
        lines->push_back(current_line);
        line curLine(meta_data.getSpaceWidth(), font_size, max_line_size);
        current_line = curLine;
        current_line.add_word(current_word);
    }
    lines->push_back(current_line);
}

void text_component::create_quad_vert(std::vector<line> lines)
{
    float curser_x = 0.0f;
    float curser_y = 0.0f;

    for (line _line : lines)
    {
        if (center_text)
        {
            curser_x = (_line.get_max_length() - _line.get_line_length()) / 2;
        }
        for (word _word : _line.get_words())
        {
            for (font_char* letter : _word.get_chars())
            {
                add_vertices_for_char(curser_x, curser_y, *letter, font_size, &vertices);
                add_mesh_info(&tex_coords, letter->get_x_tex_coord(), letter->get_y_tex_coord(), letter->get_x_max_tex_coord(), letter->get_y_max_tex_coord());
                curser_x += letter->get_x_advance() * font_size;
            }
            curser_x += meta_data.getSpaceWidth() * font_size;
        }
        curser_x = 0;
        curser_y += 0.03f * font_size;
    }
}

void text_component::add_vertices_for_char(float curser_x, float curser_y, font_char _character, float size_of_font, std::vector<float> *vertices)
{
    float x = curser_x + (_character.get_x_offset() * size_of_font);
    float y = curser_y + (_character.get_y_offset() * size_of_font);
    float x_max = x + (_character.get_x_size() * size_of_font);
    float y_max = y + (_character.get_y_size() * size_of_font);
    float correct_x = (2 * x) - 1;
    float correct_y = (-2 * y) + 1;
    float correct_x_max = (2 * x_max) - 1;
    float correct_y_max = (-2 * y_max) + 1;
    add_mesh_info(vertices, correct_x, correct_y, correct_x_max, correct_y_max);
}

void text_component::add_mesh_info(std::vector<float> *vertices, float x, float y, float max_x, float max_y)
{
    vertices->push_back(x);
    vertices->push_back(y);
    vertices->push_back(x);
    vertices->push_back(max_y);
    vertices->push_back(max_x);
    vertices->push_back(max_y);
    vertices->push_back(max_x);
    vertices->push_back(max_y);
    vertices->push_back(max_x);
    vertices->push_back(y);
    vertices->push_back(x);
    vertices->push_back(y);
}