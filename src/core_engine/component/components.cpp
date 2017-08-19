#include "components.h"
#include "../../rendering_engine/stb_image.h"


void camera_component::set_parent(entity *parent)
{
    entity_component::set_parent(parent);

    m_camera.set_transform(get_transform());
}

mesh_component::mesh_component(mesh* _mesh, mat4f* shadowMap, texture* depth_map, const std::string& tex_file_name, const std::string& normal_map)
{
    this->_mesh = _mesh;
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;
    tex = new texture(tex_file_name);
    disp_map = new texture("bricks_disp.jpg");
    if (normal_map.compare("") == 0)
    {
        has_normal_map = false;
    }
    else
    {
        this->normal_map = new texture(normal_map);
        has_normal_map = true;
    }
}

mesh_component::~mesh_component()
{
}

void mesh_component::init()
{
    tex_loc  = get_shader()->get_uni_location("model_tex");

    view_projection_loc = get_shader()->get_uni_location("view_projection");
    model_loc = get_shader()->get_uni_location("model");
    view_loc = get_shader()->get_uni_location("view");

    cut_plane_loc = get_shader()->get_uni_location("plane");

    shadow_mvp_loc = get_shader()->get_uni_location("shadow_mvp");
    shadow_tex_loc = get_shader()->get_uni_location("shadow_tex");
    if (has_normal_map)
    {
        normal_tex_loc = get_shader()->get_uni_location("normal_tex");

        disp_map_loc = get_shader()->get_uni_location("disp_tex");
    }

    get_shader()->get_light_loc();

}

void mesh_component::set_all_uni(camera& cam)
{
    glUniformMatrix4fv(view_projection_loc, 1, GL_FALSE, &cam.get_view_projection()[0][0]);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &get_transform()->get_transformation()[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &cam.get_view_matrix()[0][0]);

    glUniformMatrix4fv(shadow_mvp_loc, 1, GL_FALSE, shadow_mvp[0][0]);
    glUniform4f(cut_plane_loc, cam.get_cutting_plane().get_x(), cam.get_cutting_plane().get_y(), cam.get_cutting_plane().get_z(), cam.get_cutting_plane().get_w());

    get_shader()->set_light();
}

void mesh_component::render() const
{
    tex->bind(0);
    glUniform1i(tex_loc, 0);

    depth_map->bind(1, true);
    glUniform1i(shadow_tex_loc, 1);



    if (has_normal_map)
    {
        normal_map->bind(2);
        glUniform1i(normal_tex_loc, 2);

        disp_map->bind(3);
        glUniform1i(disp_map_loc, 3);
    }

    _mesh->draw();
}

/////////////////// ANIMATION ////////////////////
animation_component::animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map, const std::string& tex_file_name, const std::string& normal_map) :
        _mesh(file_name),
        tna_model(file_name)
{
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;
    tex = new texture("default.jpg");
}

animation_component::~animation_component()
{
    delete tex;
}

void animation_component::init()
{
    tex_loc  = get_shader()->get_uni_location("model_tex");

    view_projection_loc = get_shader()->get_uni_location("view_projection");
    model_loc = get_shader()->get_uni_location("model");
    view_loc = get_shader()->get_uni_location("ciew");

    cut_plane_loc = get_shader()->get_uni_location("plane");

    shadow_mvp_loc = get_shader()->get_uni_location("shadow_mvp");
    shadow_tex_loc = get_shader()->get_uni_location("shadow_tex");

    get_shader()->get_light_loc();

}

void animation_component::set_all_uni(camera& cam)
{
    mat4f worldMatrix = get_transform()->get_transformation();
    mat4f projectedMatrix = cam.get_view_projection() * worldMatrix;

    glUniform4f(cut_plane_loc, cam.get_cutting_plane().get_x(), cam.get_cutting_plane().get_y(), cam.get_cutting_plane().get_z(), cam.get_cutting_plane().get_w());

    glUniformMatrix4fv(view_projection_loc, 1, GL_FALSE, &cam.get_view_projection()[0][0]);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &worldMatrix[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &cam.get_view_matrix()[0][0]);

    tex->bind(0);
    glUniform1i(tex_loc, 0);

    depth_map->bind(1, true);
    glUniform1i(shadow_tex_loc, 1);

    glUniformMatrix4fv(shadow_mvp_loc, 1, GL_FALSE, shadow_mvp[0][0]);


    get_shader()->set_light();

}

float counter = 0;
float animationCounter = 0;
float leftOver = 0;
void animation_component::update(float delta)
{
    counter += 500.0f * delta;

    if(counter > 10)
    {
        animationCounter += 1;
        leftOver += fmodf(counter, 10);

        if(leftOver > 10)
        {
            animationCounter++;
            leftOver = 0;
        }

        if(animationCounter >= 20 * 2 + tna_model.m_animation[0].m_time.size())
        {
            animationCounter = 0;
        }
        else
        {
            tna_model.CalcBonesWorldMatrix(animationCounter);
            counter = 0;
        }
    }

}

void animation_component::render() const
{

    std::vector<mat4f> boneMats;

    for(int i = 0; i < tna_model.m_bones.size(); i++)
    {
        std::ostringstream name;
        name << "skinning_mat[" << i << "]";

        get_shader()->SetUniformMat4(name.str(), tna_model.m_bones[i].m_skinningMatrix);
    }

    _mesh.draw();
}

/////////////////// GUI ////////////////////
gui_component::gui_component(const std::string& file_name, mesh* Mesh, int win_width, int win_height) : win_width(win_width), win_height(win_height)
{
    use_texture_class = true;
    gui_tex = new texture(file_name);
    _mesh = Mesh;
}

gui_component::gui_component(GLuint tex, mesh* Mesh, int win_width, int win_height) : win_width(win_width), win_height(win_height)
{
    use_texture_class = false;
    _mesh = Mesh;
    this->texture_id = tex;
}

gui_component::~gui_component()
{
    delete gui_tex;
}

void gui_component::init()
{

    float scaleFactor;

    if (use_texture_class)
        scaleFactor = get_transform()->get_scale() * ((float) gui_tex->get_width() / win_width);
    else
        scaleFactor = get_transform()->get_scale() * ((float) 2048 / win_width);

    get_transform()->set_scale(scaleFactor);

    float x = (get_transform()->get_pos()->get_x() / win_width - 0.5f) * 2;
    float y = (get_transform()->get_pos()->get_y() / win_height - 0.5f) * 2;

    get_transform()->set_pos(vec3f(x, y, 0.0));



    tex_loc = get_shader()->get_uni_location("model_tex");
    model_loc = get_shader()->get_uni_location("model");


}


void gui_component::set_all_uni(camera& cam)
{
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &get_transform()->get_transformation()[0][0]);

    if (use_texture_class) {
        gui_tex->bind(0);
        glUniform1i(tex_loc, 0);
    } else {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(tex_loc, 5);

    }
}

void gui_component::render() const
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    _mesh->draw();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

/////////////////// SKYBOX ////////////////////
skybox_component::skybox_component(float size)
{
    std::vector<std::string> tex_names(file_nmes, file_nmes + sizeof(file_nmes) / sizeof(file_nmes[0]));
    tex_loc = load_cube_map(tex_names);

    _mesh = new mesh("cube");
}

skybox_component::~skybox_component()
{
    delete _mesh;

}

void skybox_component::init()
{
    view_projection_loc = get_shader()->get_uni_location("view_projection");
    cube_map_loc = get_shader()->get_uni_location("cube_map");
}

void skybox_component::set_all_uni(camera& cam)
{
    mat4f fixed_view = cam.get_view_matrix();
    fixed_view.m[3][0] = 0;
    fixed_view.m[3][1] = 0;
    fixed_view.m[3][2] = 0;

    mat4f fixed_view_projection = cam.get_projection_matrix() * fixed_view;


    glUniformMatrix4fv(view_projection_loc, 1, GL_FALSE, &fixed_view_projection[0][0]);

    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_loc);
    glUniform1i(cube_map_loc, 0);
}

void skybox_component::update(float delta)
{

}

void skybox_component::render() const
{
    _mesh->draw();
}

GLuint skybox_component::load_cube_map(std::vector<std::string> file_names)
{
    GLuint texID;
    glGenTextures(6, &texID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (int i = 0; i < 6; i++)
    {
        int width, height, numComponents;
        std::string file_name = "../res/textures/skybox/" + file_names[i] + ".png";
        unsigned char* data = stbi_load(file_name.c_str(), &width, &height, &numComponents, 4);

        if(data == NULL)
        {
            std::cerr << "Unable to load texture: " << file_name << std::endl;
        }


        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    return texID;
}