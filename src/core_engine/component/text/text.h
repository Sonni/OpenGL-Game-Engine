#ifndef GAME_ENGINE_WORD_H
#define GAME_ENGINE_WORD_H

#include <vector>
#include "meta_file.h"

class word
{
public:
    word(float font_size) : m_font_size(font_size)
	{ }

    void add_char(font_char *_character)
	{
        m_chars.push_back(_character);
        m_width += _character->get_x_advance() * m_font_size;
	}

    std::vector<font_char*> get_chars() const { return m_chars; }

    float get_word_width() const { return m_width; }
private:
	std::vector<font_char*> m_chars;
	float m_width = 0;
	float m_font_size;
};

class line
{
public:
    line(float space_width, float font_size, float max_length)
    {
        m_space_size = space_width * font_size;
        m_max_length = max_length;
    }

    bool add_word(word _word)
    {
        float additional_len = _word.get_word_width();
        additional_len += !m_words.empty() ? m_space_size : 0;
        if (m_cur_line_length + additional_len <= m_max_length)
        {
            m_words.push_back(_word);
            m_cur_line_length += additional_len;
            return true;
        }
        else
        {
            return false;
        }
    }

    float get_max_length() const { return m_max_length; }

    float get_line_length() const { return m_cur_line_length; }

    std::vector<word> get_words() const { return m_words; }
private:
    float m_max_length;
    float m_space_size;

    std::vector<word> m_words;
    float m_cur_line_length = 0;
};

#endif //GAME_ENGINE_WORD_H
