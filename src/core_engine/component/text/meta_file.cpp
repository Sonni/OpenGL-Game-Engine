#include "meta_file.h"
#include "../../util/util.h"
#include "../../io/window.h"


meta_file::meta_file(const std::string& font_name)
{
    std::string path = "../res/fonts/" + font_name + ".fnt";
    m_file.open(path);

    std::string line;
    if(!m_file.is_open())
    {
        std::cerr << "Unable to load font: " << path << std::endl;
    }

    m_ratio = (float) window::WIN_WIDTH / (float) window::WIN_HEIGHT;
    load_padding();
    load_line_size();
    int img_width = get_value_of_var("scaleW");
    load_char_data(img_width);
}

bool meta_file::process_next_line()
{
    m_values.clear();
    std::string line;
    if (!m_file.good())
        return false;

    getline(m_file, line);

    std::vector<std::string> parts = util::split_string(line, ' ');

    for (std::string part : parts) {
        std::vector<std::string> pairs = util::split_string(part, '=');
        if (pairs.size() == 2) {
            m_values[pairs[0]] = pairs[1];
        }
    }
    return true;
}

int meta_file::get_value_of_var(std::string variable)
{
    return std::atoi(m_values[variable].c_str());
}

std::vector<int> meta_file::get_values_of_var(std::string variable)
{
    std::vector<std::string> numbers = util::split_string(m_values[variable], ',');
    std::vector<int> values;
    for (int i = 0; i < numbers.size(); i++)
    {
        values.push_back(std::atoi(numbers[i].c_str()));
    }

    return values;
}

void meta_file::load_padding()
{
    process_next_line();

    m_padding = get_values_of_var("padding");

    m_padding_width = m_padding[m_PAD_LEFT] + m_padding[m_PAD_RIGHT];
    m_padding_height = m_padding[m_PAD_TOP] + m_padding[m_PAD_BOTTOM];
}


void meta_file::load_line_size()
{
    process_next_line();
    int line_height_pixels = get_value_of_var("lineHeight") - m_padding_height;
    m_verticalPerPixelSize = 0.03f / (float) line_height_pixels;
    m_horizontalPerPixelSize = m_verticalPerPixelSize / m_ratio;
}

void meta_file::load_char_data(int image_width)
{
    process_next_line();
    process_next_line();
    while (process_next_line())
    {
        font_char* c = load_char(image_width);
        if (c->get_ascii() != -1)
        {
            m_meta_data[c->get_ascii()] = c;
        }
    }
}

font_char* meta_file::load_char(int image_size)
{
    int ascii = get_value_of_var("id");
    if (ascii == m_ASCII_SPACE)
    {
        m_space_width = (get_value_of_var("xadvance") - m_padding_width) * m_horizontalPerPixelSize;
        return new font_char(-1, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f);
    }
    float x = ((float) get_value_of_var("x") + (m_padding[m_PAD_LEFT] - m_DESIRED_PADDING)) / image_size;
    float y = ((float) get_value_of_var("y") + (m_padding[m_PAD_TOP] - m_DESIRED_PADDING)) / image_size;
    int width = get_value_of_var("width") - (m_padding_width - (2 * m_DESIRED_PADDING));
    int height = get_value_of_var("height") - ((m_padding_height) - (2 * m_DESIRED_PADDING));
    float quad_width = width * m_horizontalPerPixelSize;
    float quad_height = height * m_verticalPerPixelSize;
    float x_tex_size = (float) width / image_size;
    float y_tex_size = (float) height / image_size;
    float x_offset = (get_value_of_var("xoffset") + m_padding[m_PAD_LEFT] - m_DESIRED_PADDING) * m_horizontalPerPixelSize;
    float y_offset = (get_value_of_var("yoffset") + (m_padding[m_PAD_TOP] - m_DESIRED_PADDING)) * m_verticalPerPixelSize;
    float x_advance = (get_value_of_var("xadvance") - m_padding_width) * m_horizontalPerPixelSize;
    return new font_char(ascii, x, y, x_tex_size, y_tex_size, x_offset, y_offset, quad_width, quad_height, x_advance);
}