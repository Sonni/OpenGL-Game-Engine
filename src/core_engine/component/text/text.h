#ifndef GAME_ENGINE_WORD_H
#define GAME_ENGINE_WORD_H

#include <vector>
#include "meta_file.h"

class word
{
public:
    word(float font_size) : font_size(font_size)
	{ }

    void add_char(font_char *_character)
	{
		chars.push_back(_character);
		width += _character->get_x_advance() * font_size;
	}

    std::vector<font_char*> get_chars() const { return chars; }

    float get_word_width() const { return width; }
private:
	std::vector<font_char*> chars;
	float width = 0;
	float font_size;
};

class line
{
public:
    line(float space_width, float font_size, float max_length)
    {
        this->space_size = space_width * font_size;
        this->max_length = max_length;
    }

    bool add_word(word _word)
    {
        float additional_len = _word.get_word_width();
        additional_len += !words.empty() ? space_size : 0;
        if (cur_line_length + additional_len <= max_length)
        {
            words.push_back(_word);
            cur_line_length += additional_len;
            return true;
        }
        else
        {
            return false;
        }
    }

    float get_max_length() const { return max_length; }

    float get_line_length() const { return cur_line_length; }

    std::vector<word> get_words() const { return words; }
private:
    float max_length;
    float space_size;

    std::vector<word> words;
    float cur_line_length = 0;
};

#endif //GAME_ENGINE_WORD_H
