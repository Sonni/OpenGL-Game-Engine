#ifndef INC_3D_ENGINE_LIGHT_SCATTER_HPP
#define INC_3D_ENGINE_LIGHT_SCATTER_HPP


#include "../../rendering_engine/shader/shader.hpp"
#include "water/water_fbo.h"

#include <GL/glew.h>

class light_scatter_component : public entity_component {
public:
   /* float m_exposure = 0.0034f;
    float m_decay = 1.0f;
    float m_density = 0.84f;
    float m_weight = 5.65f;*/

   float m_exposure = 0.0008f;
    float m_decay = 1.0f;
    float m_density = 0.84f;
    float m_weight = 7.65;

    water_fbo* m_fbo;
    mesh* m_mesh;

    light_scatter_component(water_fbo* fbo) : m_fbo(fbo) {
        indexed_model hud_square;
        {
            hud_square.add_vertex(1.0f, -1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(1.0f, 0.0f));
            hud_square.add_vertex(1.0f, 1.0f, 0.0f);   hud_square.add_tex_coord(vec2f(1.0f, 1.0f));
            hud_square.add_vertex(-1.0f, -1.0f, 0.0f); hud_square.add_tex_coord(vec2f(0.0f, 0.0f));
            hud_square.add_vertex(-1.0f, 1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(0.0f, 1.0f));
            hud_square.add_face(0, 1, 2); hud_square.add_face(2, 1, 3);
        }
        m_mesh = new mesh("light_scatter", hud_square.finalize());
    }

    virtual void init()
    {
        get_shader()->use_shader();

        get_shader()->add_uniform("model");
        get_shader()->add_uniform("exposure");
        get_shader()->add_uniform("decay");
        get_shader()->add_uniform("density");
        get_shader()->add_uniform("weight");
        get_shader()->add_uniform("lightPositionOnScreen");
        get_shader()->add_uniform("myTexture");

    }

    virtual void set_all_uni(camera& cam)
    {
        get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());

        get_shader()->set_uniform_1f("exposure", m_exposure);
        get_shader()->set_uniform_1f("decay", m_decay);
        get_shader()->set_uniform_1f("density", m_density);
        get_shader()->set_uniform_1f("weight", m_weight);

        mat4f trans;

        trans.init_translation(vec3f(10000, 3000, -10000));


        vec4f light_pos_scree = glToScreen(vec4f(1000, 1000, 1000, 0),  cam.get_view_projection() * trans);
        //vec2f p = get2dPoint(vec3f(10000, 10000, -1000), cam.get_view_projection(), 824, 600);
        vec4f clipSpacePos = cam.get_view_projection().transform(vec4f(10000, 10000, 10000, 1));
        vec3f ndcSpacePos = vec3f(clipSpacePos[0], clipSpacePos[1], clipSpacePos[2]) / clipSpacePos[3];
        vec3f windowSpacePos = ((vec3f(ndcSpacePos[0]+1, ndcSpacePos[1]+1, 1)) / 2.0) * vec3f(820, 600, 1);


        auto m = (cam.get_view_projection()).transform(vec3f(10, 10, 10));

        vec3f vv(vec3f(m[0], m[1], m[2]) / m[3]);
        vec2f vvv = vec2f((vv[0]+1)/2*820, (vv[1]+1)/2*600);



        mat4f cam_conj_rot_matrix = cam.get_transform()->get_rot()->negate().to_rotation_matrix();
        mat4f cam_translation_matrix;

        //*-1 to invert, because the world should appear to move
        cam_translation_matrix.init_translation(*cam.get_transform()->get_pos() * -1);

        mat4f newVp = cam.get_projection_matrix() * cam.get_view_matrix();


        vec3f vvvv = W2S(cam.get_view_projection() * trans, vec3f(0, 0, 0));

        get_shader()->set_uniform_2f("lightPositionOnScreen", vec2f(vvvv[0], vvvv[1]));

       //get_shader()->set_sampler_2d("myTexture", m_fbo->get_reflect_tex(), 0);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, m_fbo->get_reflect_tex());
        glUniform1i(get_shader()->get_uniform("myTexture"), 0);


    }


    virtual void render() const
    {
        m_mesh->draw();
    }

    virtual void update(float delta, const camera &cam) { }

    vec3f W2S(mat4f mvpmatrix, vec3f vPlayerLoc)
    {
        //Matrix-Vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
        vec4f clipCoords;
        /*clipCoords[0] = vPlayerLoc[0]*mvpmatrix[0][0] + vPlayerLoc[1]*mvpmatrix[0][1] + vPlayerLoc[2] * mvpmatrix[0][2] + mvpmatrix[0][3];
        clipCoords[1] = vPlayerLoc[0]*mvpmatrix[1][0] + vPlayerLoc[1]*mvpmatrix[1][1] + vPlayerLoc[2]*mvpmatrix[1][2] + mvpmatrix[1][3];
        clipCoords[2] = vPlayerLoc[0]*mvpmatrix[2][0] + vPlayerLoc[1]*mvpmatrix[2][1] + vPlayerLoc[2]*mvpmatrix[2][2] + mvpmatrix[2][3];
        clipCoords[3] = vPlayerLoc[0]*mvpmatrix[3][0] + vPlayerLoc[1]*mvpmatrix[3][1] + vPlayerLoc[2]*mvpmatrix[3][2] + mvpmatrix[3][3];*/

        clipCoords[0] = vPlayerLoc[0]*mvpmatrix[0][0] + vPlayerLoc[1]*mvpmatrix[1][0] + vPlayerLoc[2]*mvpmatrix[2][0] + mvpmatrix[3][0];
        clipCoords[1] = vPlayerLoc[0]*mvpmatrix[0][1] + vPlayerLoc[1]*mvpmatrix[1][1] + vPlayerLoc[2]*mvpmatrix[2][1] + mvpmatrix[3][1];
        clipCoords[2] = vPlayerLoc[0]*mvpmatrix[0][2] + vPlayerLoc[1]*mvpmatrix[1][2] + vPlayerLoc[2]*mvpmatrix[2][2] + mvpmatrix[3][2];
        clipCoords[3] = vPlayerLoc[0]*mvpmatrix[0][3] + vPlayerLoc[1]*mvpmatrix[1][3] + vPlayerLoc[2]*mvpmatrix[2][3] + mvpmatrix[3][3];

        if (clipCoords[3] < 0.1f)
            return vec3f(10, 10, 0);


        //perspective division, dividing by clip.W = NDC
        vec3f normalizedDeviceCoordinates;
        normalizedDeviceCoordinates[0] = clipCoords[0] / clipCoords[3];
        normalizedDeviceCoordinates[1] = clipCoords[1] / clipCoords[3];
        normalizedDeviceCoordinates[2] = clipCoords[2] / clipCoords[3];

        //viewport tranform to screenCooords
        GLint viewport[4] = { 0, 0, 824, 600 };
        GLfloat depthrange[2] = { 0 };
        //glGetIntegerv(GL_VIEWPORT, viewport); // viewport = (0, 0, width, height)
        glGetFloatv(GL_DEPTH_RANGE, depthrange); //depthrange = (0,1)

        vec3f playerScreenCoords;

        playerScreenCoords[0] =   normalizedDeviceCoordinates[0];
        playerScreenCoords[1] =  -normalizedDeviceCoordinates[1];

        playerScreenCoords[0] =   (viewport[2] / 2 * normalizedDeviceCoordinates[0]) + (normalizedDeviceCoordinates[0] + viewport[2] / 2);
        playerScreenCoords[1] =  (viewport[3] / 2 * normalizedDeviceCoordinates[1]) + (normalizedDeviceCoordinates[1] + viewport[3] / 2);

        float x = (float) (playerScreenCoords[0] * 2) / (float) 824 - 1.0;
        float y = (float) (playerScreenCoords[1] * 2) / (float) 600 - 1.0;



        return vec3f(x, y, 0);
    }

    vec2f get2dPoint(vec3f point3D, mat4f viewProjectionMatrix, int width, int height) {

        auto v = viewProjectionMatrix.transform(vec4f(point3D[0], point3D[1], point3D[2], 1));
        int winX = (int) (( v[0] + 1 ) / 2.0) * width ;
        //we calculate -point3D.getY() because the screen Y axis is
        //oriented top->down
        int winY = (int) (( 1 - v[1] ) / 2.0) * height ;
        return vec2f(winX, winY);
    }



    vec4f glToScreen(const vec4f& v, mat4f mvpp)
    {
        double mvp[16];
        double viewport[4];
        double depthRange[2];

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                mvp[i * 4 + j] = mvpp[i][j];
            }
        }


        viewport[0] = viewport[1] = 0.0;
        viewport[1] = 820; viewport[2] = 600;
        depthRange[0] = depthRange[1] = 1.0;

        // Compose the matrices into a single row-major transformation
        vec4f T[4];
        int r, c, i;
        for (r = 0; r < 4; ++r) {
            for (c = 0; c < 4; ++c) {
                T[r][c] = 0;
                for (i = 0; i < 4; ++i) {
                    // OpenGL matrices are column major
                    T[r][c] += mvp[i + c * 4];
                }
            }
        }

        // Transform the vertex
        vec4f result;
        for (r = 0; r < 4; ++r) {
            result[r] = T[r].dot(v);
        }

        // Homogeneous divide
        const double rhw = 1 / result.get_w();

        return vec4f(
                (1 + result.get_x() * rhw) * viewport[2] / 2 + viewport[0],
                (1 - result.get_y() * rhw) * viewport[3] / 2 + viewport[1],
                (result.get_z() * rhw) * (depthRange[1] - depthRange[0]) + depthRange[0],
                rhw);
    }
private:
};


#endif //INC_3D_ENGINE_LIGHT_SCATTER_HPP
