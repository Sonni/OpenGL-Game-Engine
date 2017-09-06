#ifndef INC_3D_ENGINE_SKELETAL_ANIMATION_H
#define INC_3D_ENGINE_SKELETAL_ANIMATION_H


#include <map>
#include <vector>
#include "../../core_engine/math/quaternion.h"

struct joint
{
public:
    int id;
    std::string name;
    int parent_id;
    mat4f animated_transform;
    mat4f inverse_bind_transform;
    std::vector<joint*> children;

    void calc_inverse_bind(const mat4f& parent_bind_pose)
    {
        mat4f bindTransform = parent_bind_pose * inverse_bind_transform;

        for (joint* child : children)
        {
            child->calc_inverse_bind(bindTransform);
        }
    }
};

struct joint_transform
{
public:
    joint_transform(const vec3f& pos, const quaternion& rot) :
            pos(pos),
            rot(rot)
    { }

    joint_transform() :
            pos(vec3f()),
            rot(quaternion())
    { }

    inline mat4f get_local_transform() const { return mat4f().init_translation(pos) * rot.to_rotation_matrix().transpose(); }
    inline vec3f get_pos() const { return pos; }
    inline quaternion get_rot() const { return rot; }

private:
    vec3f pos;
    quaternion rot;
};

struct keyframe
{
public:
    std::map<std::string, joint_transform>* poses;
    float time_stamp;
};

#endif //INC_3D_ENGINE_SKELETAL_ANIMATION_H
