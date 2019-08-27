#ifndef GAME_ENGINE_WATER_H
#define GAME_ENGINE_WATER_H


#include "../components.hpp"

class water_component : public entity_component {
public:

    water_component(int width, int breath, const std::string &dudv, const std::string &normalMap, water_fbo *wfb, camera *cam, std::vector<entity*>* waterDraws);

    ~water_component() {
        delete m_dudv_tex;
        delete m_normal_tex;
    }

    virtual void init();

    virtual void set_all_uni(camera &cam);

    virtual void update(float delta, const camera &cam);

    virtual void render() const;
    void render_reflect_refract() const;

private:
    simple_plane2 m_mesh;
    float m_moveFactor = 0.0;
    bool m_draw = false;

    aabb view_box;

    texture *m_dudv_tex;
    texture *m_normal_tex;

    water_fbo *m_wfb;

    GLint m_model_loc, m_mvp_loc;
    GLint m_dudv_loc, m_normal_loc;
    GLint m_refrac_loc, m_reflec_loc, m_depth_loc;
    GLint m_cam_pos_loc;
    GLint m_move_factor_loc;

    camera* m_cam;
    std::vector<entity*>* m_waterDraws;


    // new stuff
    GLint m_numWaves;
    GLint m_amplitude;
    GLint m_wavelength;
    GLint m_speed;
    GLint m_direction;

    float uniformRandomInRange(float min, float max);

    void render_objects(camera* cam, std::vector<entity*>* entities) const;
    void setup_reflection(camera* cam, water_fbo* wfb, float height_of_water, float y_distance) const;
    void setup_refraction(camera* cam, water_fbo* wfb, float height_of_water, float y_distance, quaternion old_cam_rot) const;
};


#endif //GAME_ENGINE_WATER_H
