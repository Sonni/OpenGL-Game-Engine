#ifndef ___D_rendering_engine__loader__
#define ___D_rendering_engine__loader__

#include "../../core_engine/math/matrix.h"
#include "../../core_engine/math/quaternion.h"
#include "joint.h"
#include <string>
#include <vector>
#include <map>



struct joint_transform
{
    vec3f pos;
    quaternion rot;

    mat4f getLocalTransform() {
        mat4f matrix;
        matrix.init_translation(pos);

        matrix = matrix * rot.to_rotation_matrix().transpose();
        return matrix;
    }
};

struct keyframe
{
    float time_stamp;
    std::map<std::string, joint_transform>* pose;
};


class Loader
{
public:
    static void LoadWeights(const std::string& line, std::vector<float>& weightReft);
    static void load_joint_ids(const std::string &line, std::vector<int> &ids);
    static void load_joints(const std::string &line, joint* root);
    static void load_keyframe(const std::string &line, std::vector<keyframe>& keyframes);



protected:
private:

};

#endif