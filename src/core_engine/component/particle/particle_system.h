#ifndef INC_3D_ENGINE_PARTICLE_SYSTEM_H
#define INC_3D_ENGINE_PARTICLE_SYSTEM_H


#include "../../entity_component.h"
#include "../../../rendering_engine/model/mesh.h"
#include "particle.h"

class particle_system : public entity_component
{
public:
    particle_system(mesh* _mesh, camera* cam, float pps, float speed, float gravityComplient, float lifeLength) : _mesh(_mesh), cam(cam)
    {
        this->pps = pps;
        this->averageSpeed = speed;
        this->gravityComplient = gravityComplient;
        this->averageLifeLength = lifeLength;

        averageScale = 0.8f;

        randomize_rotation();
        set_direction(vec3f(0, 1, 0), 0.1f);
        set_life_error(0.1f);
        set_scale_error(0.8f);
        set_speed_error(0.8f);


        srand(769);
    }

    virtual ~particle_system() {}

    virtual void init()
    {



        for (unsigned int i = 0; i < particles.size(); i++)
        {
            particles.at(i).init();
        }

    }

    virtual void set_all_uni(camera& cam)
    {
        for (unsigned int i = 0; i < particles.size(); i++)
        {
            particles[i].set_all_uni(cam);
            particles[i].render();
        }
    }


    virtual void update(float delta)
    {
        generate_particles(vec3f(40, 15, 40));

        for (unsigned int i = 0; i < particles.size(); i++)
        {
            particles[i].update(delta);
            if (particles[i].get_is_dead())
            {
                particles.erase(particles.begin()+i);
            }
        }
    }

    virtual void render() const
    {

    }

    void set_direction(vec3f direction, float deviation)
    {
        this->direction = vec3f(direction);
        this->directionDeviation = (float) (deviation * MATH_PI);
    }

    void randomize_rotation() {
        randomRotation = true;
    }

    void set_speed_error(float error) {
        this->speedError = error * averageSpeed;
    }

    void set_life_error(float error) {
        this->lifeError = error * averageLifeLength;
    }


    void set_scale_error(float error) {
        this->scaleError = error * averageScale;
    }

    void generate_particles(vec3f systemCenter)
    {
        float delta = 0.066f;
        float particlesToCreate = pps * delta;
        int count = (int) floor(particlesToCreate);
        float partialParticle = (float) fmod(particlesToCreate, 1.0f);

        for (int i = 0; i < count; i++)
        {
            emit_particle(systemCenter);
        }
        //if (Math.random() < partialParticle) { emit_particle(systemCenter); }
    }

    void emit_particle(vec3f center)
    {
        vec3f velocity;
        if(direction != vec3f()){
            velocity = generate_rand_unit_vec_within_cone(direction, directionDeviation);

        }else{
            velocity = generate_rand_unit_vec();
        }
        velocity = velocity.normalized();
        velocity.scale(generate_value(averageSpeed, speedError));
        float scale = generate_value(averageScale, scaleError);
        float lifeLength = generate_value(averageLifeLength, lifeError);

        transform t(center, quaternion(), scale);
        particle p(_mesh, *get_shader(), t, velocity, gravityComplient, lifeLength);
         p.init();
         particles.push_back(p);
    }

    float generate_value(float average, float errorMargin) {
        float offset = (get_rand_float() - 0.5f) * 2.0f * errorMargin;
        return average + offset;
    }

    float generate_rot()
    {
        if (randomRotation) {
            return get_rand_float() * 360.0f;
        } else {
            return 0;
        }
    }

    vec3f generate_rand_unit_vec_within_cone(vec3f coneDirection, float angle)
    {
        float cosAngle = (float) cos(angle);

        float theta = (float) (get_rand_float() * 2.0f * MATH_PI);
        float z = cosAngle + (get_rand_float() * (1 - cosAngle));
        float rootOneMinusZSquared = (float) sqrt(1 - z * z);
        float x = (float) (rootOneMinusZSquared * cos(theta));
        float y = (float) (rootOneMinusZSquared * sin(theta));

        vec4f direction(x, y, z, 1);
        if (coneDirection.get_x() != 0 || coneDirection.get_y() != 0 || (coneDirection.get_z() != 1 && coneDirection.get_z() != -1))
        {
            vec3f rotateAxis = coneDirection.cross(vec3f(0, 0, 1));
            rotateAxis = rotateAxis.normalized();
            float rotateAngle = (float) acos(coneDirection.dot(vec3f(0, 0, 1)));
            quaternion q(rotateAxis, -rotateAngle);
            mat4f rotationMatrix = q.to_rotation_matrix();

            direction = rotationMatrix.transform(direction);

        } else if (coneDirection.get_z() == -1) {
            direction.set_z(direction.get_z()*-1);
        }
        return vec3f(direction.get_x(), direction.get_y(), direction.get_z());
    }

    vec3f generate_rand_unit_vec()
    {
        float theta = (float) (get_rand_float() * 2.0f * MATH_PI);
        float z = (get_rand_float() * 2) - 1;
        float rootOneMinusZSquared = (float) sqrt(1 - z * z);
        float x = (float) (rootOneMinusZSquared * cos(theta));
        float y = (float) (rootOneMinusZSquared * sin(theta));
        return vec3f(x, y, z);
    }

    float get_rand_float()
    {
        return (rand() / (float)RAND_MAX);
    }

private:
    int counter = 0;
    std::vector<particle> particles;
    mesh* _mesh;
    camera* cam;

    float pps, averageSpeed, gravityComplient, averageLifeLength, averageScale;

    float speedError, lifeError, scaleError = 0;
    bool randomRotation = false;
    vec3f direction;
    float directionDeviation = 0;

};

#endif //INC_3D_ENGINE_PARTICLE_SYSTEM_H
