#include "shader.hpp"
#include "../../utils/logger.hpp"


std::string shader::get_code(const std::string& file_name)
{
    std::string shader_code;
    std::ifstream shader_stream(file_name, std::ios::in);

    if(shader_stream.is_open())
    {
        std::string line;
        while(getline(shader_stream, line))
        {
            const char* l = line.c_str();
            if (l[0] == '#' && l[1] == 'i')
            {
                const std::size_t start = line.find('<');
                const std::size_t end = line.find('>');

                const std::string include_name = line.substr(start+1, end-start-1);
                line = get_code(m_path + include_name);
            }
            shader_code += "\n" + line;
        }
        shader_stream.close();
    }
    else return "-1";

    return shader_code;
}


shader::shader(const std::string& vs_file_name, const std::string& fs_file_name, material* mat) : m_material(mat)
{
    bool found_shaders = true;

    const char* vertex_file_path = (m_path + vs_file_name).c_str();
    const char* fragment_file_path = (m_path + fs_file_name).c_str();

    const std::string vs_code = get_code(vertex_file_path);
    if (vs_code == "-1")
    {
        logger::log_err("Could not find vertex shader file at: ", vertex_file_path);
        found_shaders = false;
    }

    const std::string fs_code = get_code(fragment_file_path);
    if (vs_code == "-1")
    {
        logger::log_err("Could not find fragment shader file at: ", fragment_file_path);
        found_shaders = false;
    }

    if (found_shaders) m_program_id = load(vs_code, fs_code);
    else { m_program_id = 0; }
}

shader::shader(const std::string& glsl_file_name, material* mat) : m_material(mat)
{
    bool found_shader = true;
    const char* file_path = (m_path + glsl_file_name).c_str();
    std::string vs_code, fs_code;

    std::string* shader_code = &vs_code;
    std::ifstream shader_stream(file_path, std::ios::in);

    if(shader_stream.is_open())
    {
        std::string line;
        getline(shader_stream, line);
        *shader_code += "\n" + line;

        while(getline(shader_stream, line))
        {
            if (line == "//-END_OF_VS-")
            {
                shader_code = &fs_code;
                getline(shader_stream, line);
            }

            const char* l = line.c_str();
            if (l[0] == '#' && l[1] == 'i')
            {
                const std::size_t start = line.find('<');
                const std::size_t end = line.find('>');

                const std::string include_name = line.substr(start+1, end-start-1);
                line = get_code(m_path + include_name);
            }
            *shader_code += "\n" + line;
        }

        shader_stream.close();
    }
    else  {
        found_shader = false;
        logger::log_err("Could not find glsl file at: ", file_path);
    }

    if (found_shader)
    {
        logger::log_msg("Linking: ", glsl_file_name);

        m_program_id = load(vs_code, fs_code);
    }
    else { m_program_id = 0; }
}

GLuint shader::load(const std::string& vs_code, const std::string& fs_code)
{
    const GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);


    GLint result = GL_FALSE;
    GLsizei log_len;

    //Compile vertex shader
    char const* vertex_src_pointer = vs_code.c_str();
    glShaderSource(vertex_id, 1, &vertex_src_pointer, nullptr);
    glCompileShader(vertex_id);

    //Check Vertex Shader
    glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_id, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0)
    {
        std::vector<char> vertex_err_msg((size_t)log_len + 1);
        glGetShaderInfoLog(vertex_id, log_len, nullptr, &vertex_err_msg[0]);
        printf("%s\n", &vertex_err_msg[0]);
    }


    //Compile fragment shader
    char const* fragment_src_pointer = fs_code.c_str();
    glShaderSource(fragment_id, 1, &fragment_src_pointer, nullptr);
    glCompileShader(fragment_id);

    //Check Fragment Shader
    glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_id, GL_INFO_LOG_LENGTH, &log_len);
    if ( log_len > 0 ){
        std::vector<char> fragment_err_msg((size_t) log_len+1);
        glGetShaderInfoLog(fragment_id, log_len, nullptr, &fragment_err_msg[0]);
        printf("%s\n", &fragment_err_msg[0]);
    }


    // Link the program
    const GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0)
    {
        std::vector<char> program_err_msg((size_t) log_len + 1);
        glGetProgramInfoLog(program_id, log_len, nullptr, &program_err_msg[0]);
        printf("%s\n", &program_err_msg[0]);
    }

    glDetachShader(program_id, vertex_id);
    glDetachShader(program_id, fragment_id);

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);

    return program_id;
}

void shader::set_light_loc()
{
    add_uniform("directionalLight.base.color");
    add_uniform("directionalLight.base.intensity");
    add_uniform("directionalLight.direction");

    for(int i = 0; i < m_lc->m_MAX_POINT_LIGHTS; i++)
    {
        std::ostringstream pointLightNameBuilder;
        pointLightNameBuilder << "pointLights[" << i << "]";
        std::string pointLightName = pointLightNameBuilder.str();

        add_uniform(pointLightName + ".base.color");
        add_uniform(pointLightName + ".base.intensity");
        add_uniform(pointLightName + ".atten.constant");
        add_uniform(pointLightName + ".atten.linear");
        add_uniform(pointLightName + ".atten.exponent");
        add_uniform(pointLightName + ".position");
        add_uniform(pointLightName + ".range");
    }

    for(int i = 0; i < m_lc->m_MAX_SPOT_LIGHTS; i++)
    {
        std::ostringstream spotLightNameBuilder;
        spotLightNameBuilder << "spotLights[" << i << "]";
        std::string spotLightName = spotLightNameBuilder.str();

        add_uniform(spotLightName + ".pointLight.base.color");
        add_uniform(spotLightName + ".pointLight.base.intensity");
        add_uniform(spotLightName + ".pointLight.atten.constant");
        add_uniform(spotLightName + ".pointLight.atten.linear");
        add_uniform(spotLightName + ".pointLight.atten.exponent");
        add_uniform(spotLightName + ".pointLight.position");
        add_uniform(spotLightName + ".pointLight.range");
        add_uniform(spotLightName + ".direction");
        add_uniform(spotLightName + ".cutoff");
    }
}

void shader::set_light()
{
    set_uniform_3f("directionalLight.direction", m_lc->m_directionalLight.m_direction);
    set_uniform_3f("directionalLight.base.color", m_lc->m_directionalLight.m_base.m_color);
    set_uniform_1f("directionalLight.base.intensity", m_lc->m_directionalLight.m_base.m_intensity);

    for(int i = 0; i < m_lc->m_numPointLights; i++)
    {
        std::ostringstream pointLightNameBuilder;
        pointLightNameBuilder << "pointLights[" << i << "]";
        std::string pointLightName = pointLightNameBuilder.str();

        set_uniform_3f(pointLightName + ".base.color", m_lc->m_pointLights[i].m_base.m_color);
        set_uniform_1f(pointLightName + ".base.intensity", m_lc->m_pointLights[i].m_base.m_intensity);
        set_uniform_1f(pointLightName + ".atten.constant", m_lc->m_pointLights[i].m_atten.m_constant);
        set_uniform_1f(pointLightName + ".atten.linear", m_lc->m_pointLights[i].m_atten.m_linear);
        set_uniform_1f(pointLightName + ".atten.exponent", m_lc->m_pointLights[i].m_atten.m_exponent);
        set_uniform_3f(pointLightName + ".position", m_lc->m_pointLights[i].m_position);
        set_uniform_1f(pointLightName + ".range", m_lc->m_pointLights[i].m_range);
    }

    for(int i = 0; i < m_lc->m_numSpotLights; i++)
    {
        std::ostringstream spotLightNameBuilder;
        spotLightNameBuilder << "spotLights[" << i << "]";
        std::string spotLightName = spotLightNameBuilder.str();

        set_uniform_3f(spotLightName + ".pointLight.base.color", m_lc->m_spotLights[i].m_point_light.m_base.m_color);
        set_uniform_1f(spotLightName + ".pointLight.base.intensity", m_lc->m_spotLights[i].m_point_light.m_base.m_intensity);
        set_uniform_1f(spotLightName + ".pointLight.atten.constant", m_lc->m_spotLights[i].m_point_light.m_atten.m_constant);
        set_uniform_1f(spotLightName + ".pointLight.atten.linear", m_lc->m_spotLights[i].m_point_light.m_atten.m_linear);
        set_uniform_1f(spotLightName + ".pointLight.atten.exponent", m_lc->m_spotLights[i].m_point_light.m_atten.m_exponent);
        set_uniform_3f(spotLightName + ".pointLight.position", m_lc->m_spotLights[i].m_point_light.m_position);
        set_uniform_1f(spotLightName + ".pointLight.range", m_lc->m_spotLights[i].m_point_light.m_range);
        set_uniform_3f(spotLightName + ".direction", m_lc->m_spotLights[i].m_q.get_forward());
        set_uniform_1f(spotLightName + ".cutoff", m_lc->m_spotLights[i].m_cutoff);
    }
}