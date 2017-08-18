#include "meta_file.h"
#include "../../util/util.h"
#include "../../io/window.h"


meta_file::meta_file(const std::string& font_name)
{
    std::string path = "../res/fonts/" + font_name + ".fnt";
    file.open(path);

    std::string line;
    if(!file.is_open())
    {
        std::cerr << "Unable to load font: " << path << std::endl;
    }

    this->ratio = (float) window::WIN_WIDTH / (float) window::WIN_HEIGHT;
    load_padding();
    load_line_size();
    int img_width = get_value_of_var("scaleW");
    load_char_data(img_width);
}

bool meta_file::process_next_line()
{
    values.clear();
    std::string line;
    if (!file.good())
        return false;

    getline(file, line);

    std::vector<std::string> parts = util::split_string(line, ' ');

    for (std::string part : parts) {
        std::vector<std::string> pairs = util::split_string(part, '=');
        if (pairs.size() == 2) {
            values[pairs[0]] = pairs[1];
        }
    }
    return true;
}

int meta_file::get_value_of_var(std::string variable)
{
    return std::atoi(values[variable].c_str());
}

std::vector<int> meta_file::get_values_of_var(std::string variable)
{
    std::vector<std::string> numbers = util::split_string(values[variable], ',');
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

    this->padding = get_values_of_var("padding");

    this->padding_width = padding[PAD_LEFT] + padding[PAD_RIGHT];
    this->padding_height = padding[PAD_TOP] + padding[PAD_BOTTOM];
}


void meta_file::load_line_size()
{
    process_next_line();
    int line_height_pixels = get_value_of_var("lineHeight") - padding_height;
    verticalPerPixelSize = 0.03f / (float) line_height_pixels;
    horizontalPerPixelSize = verticalPerPixelSize / ratio;
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
            meta_data[c->get_ascii()] = c;
        }
    }
}

font_char* meta_file::load_char(int image_size)
{
    int ascii = get_value_of_var("id");
    if (ascii == ASCII_SPACE)
    {
        this->space_width = (get_value_of_var("xadvance") - padding_width) * horizontalPerPixelSize;
        return new font_char(-1, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f);
    }
    float x = ((float) get_value_of_var("x") + (padding[PAD_LEFT] - DESIRED_PADDING)) / image_size;
    float y = ((float) get_value_of_var("y") + (padding[PAD_TOP] - DESIRED_PADDING)) / image_size;
    int width = get_value_of_var("width") - (padding_width - (2 * DESIRED_PADDING));
    int height = get_value_of_var("height") - ((padding_height) - (2 * DESIRED_PADDING));
    float quad_width = width * horizontalPerPixelSize;
    float quad_height = height * verticalPerPixelSize;
    float x_tex_size = (float) width / image_size;
    float y_tex_size = (float) height / image_size;
    float x_offset = (get_value_of_var("xoffset") + padding[PAD_LEFT] - DESIRED_PADDING) * horizontalPerPixelSize;
    float y_offset = (get_value_of_var("yoffset") + (padding[PAD_TOP] - DESIRED_PADDING)) * verticalPerPixelSize;
    float x_advance = (get_value_of_var("xadvance") - padding_width) * horizontalPerPixelSize;
    return new font_char(ascii, x, y, x_tex_size, y_tex_size, x_offset, y_offset, quad_width, quad_height, x_advance);
}