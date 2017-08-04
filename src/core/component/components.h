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

    GLint disp_map_loc;
    GLint normal_tex_loc;
    GLint cut_plane_loc;
    GLint tex_loc;
    GLint model_loc, view_loc, view_projection_loc;
    GLint shadow_mvp_loc, shadow_tex_loc;
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
    animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map);
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

    GLint model_loc, view_loc, view_projection_loc;
    GLint tex_loc;
    GLint cut_plane_loc;

    GLint shadow_mvp_loc, shadow_tex_loc;
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
    GLint view_projection_loc;
    GLint cube_map_loc;

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

    GLint tex_loc, model_loc;
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
        tex_loc = get_shader()->get_uni_location("model_tex");
    }
    virtual void set_all_uni(camera& cam)
    {
        tex->bind(0);
        glUniform1i(tex_loc, 0);
    }

    virtual void render() const
    {
        _mesh->draw();
    }


private:
    mesh* _mesh;
    texture* tex;

    GLint tex_loc;
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
        tex_loc = get_shader()->get_uni_location("model_tex");
        blur_loc = get_shader()->get_uni_location("blur");
    }
    virtual void set_all_uni(camera& cam)
    {
        glUniform2f(blur_loc, blur->get_x(), blur->get_y());

        tex->bind(0);
        glUniform1i(tex_loc, 0);
    }

    virtual void render() const
    {
        _mesh->draw();
    }


private:
    mesh* _mesh;
    texture* tex;
    vec2f* blur;

    GLint tex_loc;
    GLint blur_loc;
};

/////////////////// PARTICLE RENDERER ////////////////////
class particle : public entity_component
{
public:

    particle(mesh* _mesh, vec3f velocity, float gravityEffect, float lifeLength, float scale) :
             velocity(velocity), gravity_effect(gravityEffect), life_length(lifeLength)
    {
        this->_mesh = _mesh;
    }

    ~particle() { }

    virtual void init()
    {
        model_loc = get_shader()->get_uni_location("model");
        view_projection_loc = get_shader()->get_uni_location("view_projection");
    }

    virtual void set_all_uni(camera& cam)
    {
        mat4f modelMatrix;
        modelMatrix.init_identity();
        mat4f viewMatrix = cam.get_view_matrix();
        mat4f translationMatrix;
        translationMatrix.init_translation(*get_transform()->get_pos());
        modelMatrix = modelMatrix * translationMatrix;


        modelMatrix.m[0][0] = viewMatrix.m[0][0];
        modelMatrix.m[0][1] = viewMatrix.m[1][0];
        modelMatrix.m[0][2] = viewMatrix.m[2][0];
        modelMatrix.m[1][0] = viewMatrix.m[0][1];
        modelMatrix.m[1][1] = viewMatrix.m[1][1];
        modelMatrix.m[1][2] = viewMatrix.m[2][1];
        modelMatrix.m[2][0] = viewMatrix.m[0][2];
        modelMatrix.m[2][1] = viewMatrix.m[1][2];
        modelMatrix.m[2][2] = viewMatrix.m[2][2];

        mat4f rotationMatrix;
        rotationMatrix.init_rotation_euler(get_transform()->get_rot()->get_x(), get_transform()->get_rot()->get_y(), get_transform()->get_rot()->get_z());
        modelMatrix = modelMatrix * rotationMatrix;
        //modelMatrix.rotate(40, Vector3f(0, 0, 1));

        mat4f scaleMatrix;
        scaleMatrix.init_scale(vec3f(get_transform()->get_scale(), get_transform()->get_scale(), get_transform()->get_scale()));
        modelMatrix = modelMatrix * scaleMatrix;
        //modelMatrix.scale(Vector3f(scale, scale, scale));

        mat4f modelViewMatrix = cam.get_projection_matrix() * modelMatrix;

        mat4f test = cam.get_projection_matrix() * get_transform()->get_transformation();
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(view_projection_loc, 1, GL_FALSE, &cam.get_view_projection()[0][0]);


    }

    virtual void update(float delta)
    {
        elapsed_time++;
        velocity.set_y(-10 * gravity_effect * delta);
        vec3f change = velocity;
        //change.scale(DisplayManager.getFrameTimeSeconds());
       // get_transform()->set_pos(change + *get_transform()->get_pos());
        //return elapsedTime < m_lifeLength;
        if (elapsed_time > 50)
        {
            //delete this;
        }
    }

    virtual void render() const
    {
        glDisable(GL_CULL_FACE);
        _mesh->draw();
        glEnable(GL_CULL_FACE);
    }


private:
    mesh* _mesh;
    vec3f velocity;
    float gravity_effect;
    float life_length;
    float elapsed_time = 0;

    GLint model_loc, view_projection_loc;
};

class particle_system : public entity_component
{
public:
    particle_system(mesh _mesh) : _mesh(_mesh)
    {
        entity* particle = new entity(new shader("particle.glsl"));
        // particle->add_component(new Particle(m_mesh, vec3f(0, 2, 0), 0.5f, 4.0, 50));

       // particles.push_back(particle);
    }

    virtual ~particle_system() {}

    virtual void init()
    {
        for (unsigned int i = 0; i < particles.size(); i++)
        {

        }
    }

    virtual void set_all_uni(camera& cam)
    {
        for (unsigned int i = 0; i < particles.size(); i++)
        {
            particles[i]->set_all_uni(cam);
        }
    }


    virtual void update(float delta)
    {
        for (unsigned int i = 0; i < particles.size(); i++)
        {
            //particles[i].update(delta);
        }
    }

    virtual void render() const
    {
        for (unsigned int i = 0; i < particles.size(); i++)
        {
            particles[i]->render();
        }
    }

private:
    int counter = 0;
    std::vector<entity*> particles;
    mesh _mesh;

};



#endif //GAME_ENGINE_COMPONENTS_H
