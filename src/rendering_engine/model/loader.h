#ifndef ___D_rendering_engine__loader__
#define ___D_rendering_engine__loader__

#include "skeletal_animation.h"

class Loader
{
public:
    static void load_weights(const std::string &line, std::vector<float> &weightReft);
    static void load_joint_ids(const std::string &line, std::vector<int> &ids);
    static void load_joints(const std::string &line, joint* root);
    static void load_keyframe(const std::string &line, std::vector<keyframe>& keyframes);

protected:
private:

};

#endif