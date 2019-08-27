#ifndef GAME_ENGINE_COMPONENTS_H
#define GAME_ENGINE_COMPONENTS_H

#include "../entity.hpp"
#include "../../rendering_engine/model/mesh.hpp"
#include "../../rendering_engine/shader/shader.hpp"
#include "../../rendering_engine/model/tna_loader.hpp"
#include "../../rendering_engine/texture.hpp"
#include "water/water_fbo.h"
#include <sstream>

class camera_component : public entity_component
{
public:
    camera_component(const mat4f& projection)
            {}


    virtual void set_parent(entity* parent);
protected:
private:
};

/////////////////// SKYBOX ////////////////////
class skybox_component : public entity_component
{
public:
    skybox_component();
    void operator=(const skybox_component& skyboxRenderer) { }

    ~skybox_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void update(float delta, const camera &cam);
    virtual void render() const;


private:
    GLuint load_cube_map(std::vector<std::string> file_names);

    GLuint m_tex_loc;
    simple_cube m_cube;
    std::string m_file_names[6] = {"right", "left", "top", "bottom", "back", "front"};
};


/////////////////// Post processor ////////////////////
class post_processing_component : public entity_component
{
public:
    post_processing_component(texture* target, mesh* Mesh) : m_tex(target), m_mesh(Mesh)
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
        m_tex->bind(0);
        get_shader()->set_uniform_1i("model_tex", 0);
    }

    virtual void render() const
    {
        m_mesh->draw();
    }


private:
    mesh* m_mesh;
    texture*m_tex;
};

/////////////////// guassian blur ////////////////////
class blur_component : public entity_component
{
public:
    blur_component(texture* target, mesh* Mesh, vec2f* blur) : m_tex(target), m_mesh(Mesh), m_blur(blur)
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
        get_shader()->set_uniform_2f("blur", *m_blur);

        m_tex->bind(0);
        get_shader()->set_uniform_1i("model_tex", 0);
    }

    virtual void render() const
    {
        m_mesh->draw();
    }


private:
    mesh* m_mesh;
    texture* m_tex;
    vec2f* m_blur;
};

#endif //GAME_ENGINE_COMPONENTS_H
