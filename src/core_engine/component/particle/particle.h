#ifndef INC_3D_ENGINE_PARTICLE_H
#define INC_3D_ENGINE_PARTICLE_H


#include "../../../rendering_engine/model/mesh.h"
#include "../../../shader/shader.h"
#include "../../../rendering_engine/texture.h"

class particle
{
public:

    particle(mesh* _mesh, const shader& _shader, transform _transform, vec3f velocity, float gravityEffect, float lifeLength) :
            _mesh(_mesh), _transform(_transform), _shader(_shader), velocity(velocity), gravity_effect(gravityEffect), life_length(lifeLength), rotation(rotation)
    {
        tex = new texture("particle/particleStar.png");


        std::vector<float> verticies;
        verticies.push_back(-0.5f); verticies.push_back(0.5f); verticies.push_back(-0.5f); verticies.push_back(-0.5f);
        verticies.push_back(0.5f); verticies.push_back(0.5f); verticies.push_back(0.5f); verticies.push_back(-0.5f);

        vertex_count = (int) (verticies.size() / 2);


        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);
        set_attribute(0, 2, verticies);
        glBindVertexArray(0);
    }

    void set_attribute(int attributeNumber, int coordinateSize, std::vector<float> data)
    {
        GLuint vboID;
        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
        glVertexAttribPointer(attributeNumber, coordinateSize, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    //~particle() { }

    void init()
    {

        model_loc = _shader.get_uni_location("model");
        view_projection_loc = _shader.get_uni_location("view_projection");
        tex_loc = _shader.get_uni_location("model_tex");
    }

    void set_all_uni(camera& cam)
    {
        mat4f modelMatrix;
        modelMatrix.init_identity();
        mat4f viewMatrix = cam.get_view_matrix();
        mat4f translationMatrix;
        translationMatrix.init_translation(*_transform.get_pos());
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
        rotationMatrix.init_rotation_euler(_transform.get_rot()->get_x(), _transform.get_rot()->get_y(), _transform.get_rot()->get_z());
        modelMatrix = modelMatrix * rotationMatrix;

        mat4f scaleMatrix;
        scaleMatrix.init_scale(vec3f(_transform.get_scale(), _transform.get_scale(), _transform.get_scale()));
        modelMatrix = modelMatrix * scaleMatrix;

        mat4f modelViewMatrix = cam.get_projection_matrix() * modelMatrix;

        mat4f test = cam.get_projection_matrix() * _transform.get_transformation();
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(view_projection_loc, 1, GL_FALSE, &cam.get_view_projection()[0][0]);

        tex->bind(0);
        glUniform1i(tex_loc, 0);

    }

    void update(float delta)
    {
        elapsed_time++;
        velocity.set_y(velocity.get_y() + gravity_effect * delta);
        vec3f change = velocity * 0.008f;
        _transform.set_pos(change + *_transform.get_pos());

        if (elapsed_time > 150)
        {
            is_dead = true;
        }
    }

    void render() const
    {

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);

        glBindVertexArray(vao_id);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }


    bool get_is_dead() const { return is_dead; }


private:
    mesh* _mesh;
    texture* tex;
    shader _shader;
    vec3f velocity;
    transform _transform;
    float gravity_effect;
    float life_length;
    float elapsed_time = 0;
    float rotation;
    bool is_dead = false;

    GLint model_loc, view_projection_loc;
    GLint tex_loc;

    GLuint vao_id;
    int vertex_count;

};

#endif //INC_3D_ENGINE_PARTICLE_H
