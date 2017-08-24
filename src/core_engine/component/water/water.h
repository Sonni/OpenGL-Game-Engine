#ifndef GAME_ENGINE_WATER_H
#define GAME_ENGINE_WATER_H


#include "../components.h"

class water_component : public entity_component
{
public:

    water_component(const std::string& dudv, const std::string& normalMap, water_fbo* wfb);

    ~water_component() { delete m_mesh; delete dudv_tex; delete normal_tex; delete _sphere;}

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void update(float delta, const camera &cam);
    virtual void render() const;

private:
    mesh* m_mesh;
    float moveFactor = 0.0;
    bool draw = true;

    sphere* _sphere;

    texture* dudv_tex;
    texture* normal_tex;

    water_fbo* wfb;

    GLint model_loc, mvp_loc;
    GLint dudv_loc, normal_loc;
    GLint refrac_loc, reflec_loc, depth_loc;
    GLint cam_pos_loc;
    GLint move_factor_loc;
};


#endif //GAME_ENGINE_WATER_H
