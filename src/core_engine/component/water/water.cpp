#include "water.hpp"

// TODO update AABB to be generated from get_transform() – change entity to invoke a specific
// function on entity_component when it just have been added to the entity.
water_component::water_component(int width, int breath, const std::string& dudv, const std::string& normalMap, water_fbo* wfb, camera* cam, std::vector<entity*>* waterDraws) :
m_cam(cam), m_waterDraws(waterDraws)
{

    vec3f pos = vec3f(0.0f, 1.0f, 0.0f);
    view_box = aabb(vec3f(pos[0], pos[1]-20, pos[2]),
                    vec3f(pos[0]+width, pos[1]+20, pos[2]+breath));
    m_dudv_tex = new texture(dudv);
    m_normal_tex = new texture(normalMap);
    m_wfb = wfb;
    m_mesh = simple_plane2(pos, width, breath, 80, "plane2");
}

void water_component::init()
{
    get_shader()->add_uniform("model");
    get_shader()->add_uniform("mvp");

    get_shader()->add_uniform("dudv_tex");
    get_shader()->add_uniform("normal_tex");

    get_shader()->add_uniform("move_factor");

    get_shader()->add_uniform("refraction_tex");
    get_shader()->add_uniform("reflection_tex");
    get_shader()->add_uniform("depth_tex");
    get_shader()->add_uniform("cam_pos");


    // new stuff
    get_shader()->add_uniform("numWaves");
    for (int i = 0; i < 4; ++i) {
        get_shader()->add_uniform("amplitude[" + std::to_string(i) + "]");

        get_shader()->add_uniform("wavelength[" + std::to_string(i) + "]");

        get_shader()->add_uniform("speed[" + std::to_string(i) + "]");

        get_shader()->add_uniform("direction[" + std::to_string(i) + "]");
    }
}

float water_component::uniformRandomInRange(float min, float max) {
    assert(min < max);
    double n = (double) rand() / (double) RAND_MAX;
    double v = min + n * (max - min);
    return v;
}


void water_component::set_all_uni(camera& cam)
{
    mat4f mvp_matrix = cam.get_view_projection() * get_transform()->get_transformation();

    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_mat4f("mvp", mvp_matrix);

    get_shader()->set_uniform_3f("cam_pos", *cam.get_transform()->get_pos());

    get_shader()->set_sampler_2d("dudv_tex", m_dudv_tex, 0);

    get_shader()->set_sampler_2d("normal_tex", m_normal_tex, 1);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, m_wfb->get_reflect_tex());
    glUniform1i(get_shader()->get_uniform("reflection_tex"), 2);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, m_wfb->get_refract_tex());
    glUniform1i(get_shader()->get_uniform("refraction_tex"), 3);

    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, m_wfb->get_refract_depth_tex());
    glUniform1i(get_shader()->get_uniform("depth_tex"), 4);

    m_moveFactor += 0.02f;
    //m_moveFactor = fmodf(m_moveFactor, 1.0f);
    get_shader()->set_uniform_1f("move_factor", m_moveFactor);

    // New stuff

    get_shader()->set_uniform_1i("numWaves", 4);
    for (int i = 0; i < 4; ++i) {
        float amplitude = 10.0f / (i + 1);
        //get_shader()->set_uniform_1f("amplitude[" + std::to_string(i) + "]", amplitude);
        get_material()->set_float("amplitude[" + std::to_string(i) + "]", amplitude);


        float wavelength = 60 * M_PI / (i + 1);
        //get_shader()->set_uniform_1f("wavelength[" + std::to_string(i) + "]", wavelength);
        get_material()->set_float("wavelength[" + std::to_string(i) + "]", wavelength);

        float speed = 10.0f + 2*i;
        get_shader()->set_uniform_1f("speed[" + std::to_string(i) + "]", speed);

        float angle = uniformRandomInRange(-M_PI/3, M_PI/3);
        get_shader()->set_uniform_2f("direction[" + std::to_string(i) + "]", vec2f(cos(30), sin(40)));
    }

}

void water_component::update(float delta, const camera &cam)
{
    m_draw = cam.get_frustum().aabb_in_frustum(view_box)  == frustum::INSIDE;

}

void water_component::render() const
{
    if (m_draw)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        m_mesh.render();
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }
}

void water_component::render_reflect_refract() const {
    /// Render water
    if (m_draw) {
        float height_of_water = get_transform().get_pos().get_y();
        float y_distance = 2 * (m_cam->get_transform()->get_pos()->get_y() - height_of_water);
        quaternion old_cam_rot = m_cam->invert_pitch(*m_cam->get_transform()->get_rot());

        setup_reflection(m_cam, m_wfb, height_of_water, y_distance);
        render_objects(m_cam, m_waterDraws);

        setup_refraction(m_cam, m_wfb, height_of_water, y_distance, old_cam_rot);
        render_objects(m_cam, m_waterDraws);
        glDisable(GL_CLIP_DISTANCE0);
        m_cam->set_cutting_plane(vec4f(0, -1, 0, 10000000));
    }
    /////////////////////////////////////////////////////////////////////////////
}

void water_component::render_objects(camera* cam, std::vector<entity*>* entities) const
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (entity* e: *entities)
    {
        if (e->get_shader() != nullptr)
        {
            e->get_shader()->use_shader();
            e->set_all_uni(*cam);

            e->render();
        }
    }
}

void water_component::setup_reflection(camera* cam, water_fbo* wfb, float height_of_water, float y_distance) const
{
    glEnable(GL_CLIP_DISTANCE0);

    cam->set_cutting_plane(vec4f(0, 1, 0, -height_of_water + 1));

    wfb->bind_reflect_fbo();
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    cam->get_transform()->get_pos()->set_y(cam->get_transform()->get_pos()->get_y() - y_distance);
}

void water_component::setup_refraction(camera* cam, water_fbo* wfb, float height_of_water, float y_distance, quaternion old_cam_rot) const
{
    cam->get_transform()->get_pos()->set_y(cam->get_transform()->get_pos()->get_y() + y_distance);
    cam->reverse_pitch(old_cam_rot);

    cam->set_cutting_plane(vec4f(0, -1, 0, height_of_water));

    wfb->bind_refract_fbo();
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
