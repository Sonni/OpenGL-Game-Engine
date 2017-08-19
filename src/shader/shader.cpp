#include "shader.h"
#include <vector>
#include <iostream>

std::string path = "../src/shader/glsl/";

std::string get_code(std::string file_name)
{
    std::string shader_code;
    std::ifstream shader_stream(file_name, std::ios::in);

    if(shader_stream.is_open())
    {
        std::string Line = "";
        while(getline(shader_stream, Line))
            shader_code += "\n" + Line;
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
            *shader_code += "\n" + Line;
        }

        shader_stream.close();
    }else{
        found_shader = false;
        printf("Could not find glsl file at: %s", file_path);
    }

    if (found_shader)
        program_id = load(vs_code, fs_code);
    else
        program_id = 0;
}




GLuint shader::load(std::string vs_code, std::string fs_code)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);



    GLint Result = GL_FALSE;
    int InfoLogLength;

    //compile vertex shader
    char const * VertexSourcePointer = vs_code.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }


    //Compile fragment shader
    char const * FragmentSourcePointer = fs_code.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }



    // Link the program
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

void shader::get_light_loc()
{
    for (int i = 0; i < 4; i++)
    {
        std::string color = "light_color[" + std::to_string(i) + "]";
        std::string pos = "light_pos[" + std::to_string(i) + "]";
        std::string att = "attenuation[" + std::to_string(i) + "]";

        light_color.push_back(get_uni_location(color.c_str()));
        light_pos.push_back(get_uni_location(pos.c_str()));
        attenuation.push_back(get_uni_location(att.c_str()));
    }
}

void shader::set_light() const
{
    if (lights == nullptr)
        return;

    for(int i = 0; i < 4; i++)
    {
        if (i < lights->size())
        {
            glUniform3f(light_pos.at(i), lights->at(i).get_pos().get_x(), lights->at(i).get_pos().get_y(), lights->at(i).get_pos().get_z());
            glUniform3f(light_color.at(i), lights->at(i).get_color().get_x(), lights->at(i).get_color().get_y(), lights->at(i).get_color().get_z());
            glUniform3f(attenuation.at(i), lights->at(i).get_attenuation().get_x(), lights->at(i).get_attenuation().get_y(), lights->at(i).get_attenuation().get_z());
        }
        else
        {
            glUniform3f(light_pos.at(i), 0, 0, 0);
            glUniform3f(light_color.at(i), 0, 0, 0);
            glUniform3f(attenuation.at(i), 1, 0, 0);
        }
    }
}