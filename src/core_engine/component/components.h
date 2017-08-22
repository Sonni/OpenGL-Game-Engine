#ifndef GAME_ENGINE_COMPONENTS_H
#define GAME_ENGINE_COMPONENTS_H

#include "../entity.h"
#include "../../rendering_engine/model/mesh.h"
#include "../../shader/shader.h"
#include "../../rendering_engine/model/tnaLoader.h"
#include "../../rendering_engine/texture.h"
#include "water/water_fbo.h"
#include <sstream>


class mesh_component : public entity_component
{
public:
    mesh_component(mesh* _mesh, mat4f* shadowMap, texture* depth_map, const std::string& tex_file_name = "default.jpg", const std::string& normal_map = "");
    ~mesh_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void render() const;

private:
    mesh* _mesh;
    texture* tex;
    texture* depth_map;
    texture* normal_map;
    texture* disp_map;
    bool has_normal_map;

    mat4f* shadow_mvp;
};

class camera_component : public entity_component
{
public:
    camera_component(const mat4f& projection) :
            m_camera(projection, 0) {}


    inline mat4f get_view_projection() const { return m_camera.get_view_projection(); }

    inline void set_projection(const mat4f& projection) { m_camera.set_projection(projection); }
    virtual void set_parent(entity* parent);
protected:
private:
    camera m_camera;
};

/////////////////// Animation ////////////////////
class animation_component : public entity_component
{
public:
    animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map, const std::string& tex_file_name, const std::string& normal_map);
    ~animation_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);

    virtual void update(float delta);
    virtual void render() const;

protected:
private:
    mesh _mesh;
    TNAModel tna_model;
    texture* tex;

    texture* depth_map;
    mat4f* shadow_mvp;
};

/////////////////// SKYBOX ////////////////////
class skybox_component : public entity_component
{
public:
    skybox_component(float size);
    void operator=(const skybox_component& skyboxRenderer) { }

    ~skybox_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void update(float delta);
    virtual void render() const;


private:
    GLuint load_cube_map(std::vector<std::string> file_names);
    mesh* _mesh;

    GLuint tex_loc;

    std::string file_nmes[6] = {"right", "left", "top", "left", "back", "front"};
};

/////////////////// GUI RENDERER ////////////////////
class gui_component : public entity_component
{
public:
    gui_component(const std::string& file_name, mesh* Mesh, int win_width, int win_height);
    gui_component(GLuint tex, mesh* Mesh, int win_width, int win_height);

    ~gui_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void render() const;

private:
    mesh* _mesh;
    texture* gui_tex;
    int win_width, win_height;

    GLuint texture_id;
    bool use_texture_class;
};

/////////////////// Post processor ////////////////////
class post_processing_component : public entity_component
{
public:
    post_processing_component(texture* target, mesh* Mesh) : tex(target), _mesh(Mesh)
    {

    }
    ~post_processing_component()
    {

    }

    virtual void init()
    {
        get_shader()->add_uniform("model_tex");
    }
    virtual void set_all_uni(camera& cam)
    {
        tex->bind(0);
        get_shader()->set_uniform_1i("model_tex", 0);
    }

    virtual void render() const
    {
        _mesh->draw();
    }


private:
    mesh* _mesh;
    texture* tex;
};

/////////////////// guassian blur ////////////////////
class blur_component : public entity_component
{
public:
    blur_component(texture* target, mesh* Mesh, vec2f* blur) : tex(target), _mesh(Mesh), blur(blur)
    {

    }
    ~blur_component()
    {

    }

    virtual void init()
    {
        get_shader()->add_uniform("model_tex");
        get_shader()->add_uniform("blur");
    }
    virtual void set_all_uni(camera& cam)
    {
        get_shader()->set_uniform_2f("blur", *blur);

        tex->bind(0);
        get_shader()->set_uniform_1i("model_tex", 0);
    }

    virtual void render() const
    {
        _mesh->draw();
    }


private:
    mesh* _mesh;
    texture* tex;
    vec2f* blur;
};

#endif //GAME_ENGINE_COMPONENTS_H
