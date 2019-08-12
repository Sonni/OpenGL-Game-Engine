#include "shader.h"
#include "../utils/logger.h"

std::string path = "../res/glsl/";

std::string get_code(std::string file_name)
{
    std::string shader_code;
    std::ifstream shader_stream(file_name, std::ios::in);

    if(shader_stream.is_open())
    {
        std::string Line = "";
        while(getline(shader_stream, Line))
        {
            const char* l = Line.c_str();
            if (l[0] == '#' && l[1] == 'i')
            {
                std::size_t start = Line.find('<');
                std::size_t end = Line.find('>');

                std::string include_name = Line.substr(start+1, end-start-1);
                Line = get_code(path + include_name);
            }
            shader_code += "\n" + Line;
        }
        shader_stream.close();
    }
    else
    {
        return "-1";
    }
    return shader_code;
}


shader::shader(std::string vs_file_name, std::string fs_file_name)
{
    bool found_shaders = true;

    const char* vertex_file_path = (path + vs_file_name).c_str();
    const char* fragment_file_path = (path + fs_file_name).c_str();


    std::string vs_code = get_code(vertex_file_path);
    if (vs_code.compare("-1") == 0)
    {
        printf("Could not find vertex shader file at: %s", vertex_file_path);
        found_shaders = false;
    }

    std::string fs_code = get_code(fragment_file_path);
    if (vs_code.compare("-1") == 0)
    {
        printf("Could not find fragment shader file at: %s", fragment_file_path);
        found_shaders = false;
    }

    if (found_shaders)
        program_id = load(vs_code, fs_code);
    else
        program_id = 0;
}

shader::shader(std::string glsl_file_name)
{
    bool found_shader = true;
    const char* file_path = (path + glsl_file_name).c_str();
    std::string vs_code, fs_code;

    std::string* shader_code = &vs_code;
    std::ifstream shader_stream(file_path, std::ios::in);

    if(shader_stream.is_open())
    {
        std::string Line = "";
        getline(shader_stream, Line);
        *shader_code += "\n" + Line;

        while(getline(shader_stream, Line))
        {
            if (Line.compare("//-END_OF_VS-") == 0)
            {
                shader_code = &fs_code;
                getline(shader_stream, Line);
            }

            const char* l = Line.c_str();
            if (l[0] == '#' && l[1] == 'i')
            {
                std::size_t start = Line.find('<');
                std::size_t end = Line.find('>');

                std::string include_name = Line.substr(start+1, end-start-1);
                Line = get_code(path + include_name);
            }
            *shader_code += "\n" + Line;
        }

        shader_stream.close();
    }else{
        found_shader = false;
        logger::log_err("Could not find glsl file at: ", file_path);
    }

    if (found_shader)
    {
        logger::log_msg("Linking: ", glsl_file_name);

        program_id = load(vs_code, fs_code);
    }
    else
        program_id = 0;
}

GLuint shader::load(std::string vs_code, std::string fs_code)
{
    GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);


    GLint result = GL_FALSE;
    int log_len;

    //Compile vertex shader
    char const* vertex_src_pointer = vs_code.c_str();
    glShaderSource(vertex_id, 1, &vertex_src_pointer , NULL);
    glCompileShader(vertex_id);

    //Check Vertex Shader
    glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_id, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0)
    {
        std::vector<char> vertex_err_msg(log_len+1);
        glGetShaderInfoLog(vertex_id, log_len, NULL, &vertex_err_msg[0]);
        printf("%s\n", &vertex_err_msg[0]);
    }


    //Compile fragment shader
    char const* fragment_src_pointer = fs_code.c_str();
    glShaderSource(fragment_id, 1, &fragment_src_pointer , NULL);
    glCompileShader(fragment_id);

    //Check Fragment Shader
    glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_id, GL_INFO_LOG_LENGTH, &log_len);
    if ( log_len > 0 ){
        std::vector<char> fragment_err_msg(log_len+1);
        glGetShaderInfoLog(fragment_id, log_len, NULL, &fragment_err_msg[0]);
        printf("%s\n", &fragment_err_msg[0]);
    }


    // Link the program
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0)
    {
        std::vector<char> program_err_msg(log_len + 1);
        glGetProgramInfoLog(program_id, log_len, NULL, &program_err_msg[0]);
        printf("%s\n", &program_err_msg[0]);
    }

    glDetachShader(program_id, vertex_id);
    glDetachShader(program_id, fragment_id);

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    return program_id;
}

