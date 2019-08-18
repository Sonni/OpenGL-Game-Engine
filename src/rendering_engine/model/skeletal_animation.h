#ifndef INC_3D_ENGINE_SKELETAL_ANIMATION_H
#define INC_3D_ENGINE_SKELETAL_ANIMATION_H

#include <map>
#include <vector>
#include "../../core_engine/math/quaternion.h"

struct joint
{
public:
    int m_id;
    std::string m_name;
    int m_parent_id;
    mat4f m_animated_transform;
    mat4f m_inverse_bind_transform;
    std::vector<joint*> m_children;

    void calc_inverse_bind(const mat4f& parent_bind_pose)
    {
        mat4f bindTransform = parent_bind_pose * m_inverse_bind_transform;

        for (joint* child : m_children)
        {
            child->calc_inverse_bind(bindTransform);
        }
    }
};

struct joint_transform
{
public:
    joint_transform(const vec3f& pos, const quaternion& rot) :
            m_pos(pos),
            m_rot(rot)
    { }

    joint_transform() :
            m_pos(vec3f()),
            m_rot(quaternion())
    { }

    inline mat4f get_local_transform() const { return mat4f().init_translation(m_pos) * m_rot.to_rotation_matrix().transpose(); }
    inline vec3f get_pos() const { return m_pos; }
    inline quaternion get_rot() const { return m_rot; }

private:
    vec3f m_pos;
    quaternion m_rot;
};

struct keyframe
{
public:
    std::map<std::string, joint_transform>* m_poses;
    float m_time_stamp;
};

#endif //INC_3D_ENGINE_SKELETAL_ANIMATION_H
