#include "loader.h"
#include "../../core_engine/util/util.h"



void Loader::load_weights(const std::string &line, std::vector<float> &weightsRef)
{
    std::string weight = line;
    weight.erase(0, 3);
    
    std::vector<std::string> weightArray = util::split_string(weight, ' ');
    for(int i = 0; i < weightArray.size(); i++)
        weightsRef.push_back(stof(weightArray[i]));
}

void Loader::load_joint_ids(const std::string &line, std::vector<int> &ids)
{
    std::string j = line;
    j.erase(0, 6);

    std::vector<std::string> jArray = util::split_string(j, ' ');
    for(int i = 0; i < jArray.size(); i++)
        ids.push_back(stoi(jArray[i]));
}

bool set_children(joint* child, joint* parent)
{
    if (child->parent_id == parent->id)
    {
        parent->children.push_back(child);

        return true;
    }
    for (joint* j : parent->children)
    {
        if (set_children(child, j))
            return true;
    }

    return false;
}

void Loader::load_joints(const std::string &line, joint* root)
{
    std::string j = line;
    j.erase(0, 7);

    std::vector<std::string> s = util::split_string(j, ' ');

    mat4f m;
    m[0][0] = stof(s[6]); m[1][0] = stof(s[7]); m[2][0] = stof(s[8]); m[3][0] = stof(s[9]);
    m[0][1] = stof(s[10]); m[1][1] = stof(s[11]); m[2][1] = stof(s[12]); m[3][1] = stof(s[13]);
    m[0][2] = stof(s[14]); m[1][2] = stof(s[15]); m[2][2] = stof(s[16]); m[3][2] = stof(s[17]);
    m[0][3] = stof(s[18]); m[1][3] = stof(s[19]); m[2][3] = stof(s[20]); m[3][3] = stof(s[21]);


    joint* r = new joint();
    r->inverse_bind_transform = m.transpose();
    r->id = stoi(s[2]);
    r->parent_id = stoi(s[4]);
    r->name = s[0];

    if (r->parent_id == -1)
    {
        *root = *r;
    }
    else
    {
        set_children(r, root);
    }
}

void load_joint(std::string l, std::map<std::string, joint_transform>* m)
{
    std::vector<std::string> s = util::split_string(l, ' ');

    std::string joint_name = s[1];


    (*m)[joint_name] = joint_transform(vec3f(stof(s[3]), stof(s[4]), stof(s[5])), quaternion(stof(s[7]), stof(s[8]), stof(s[9]), stof(s[10])));
}

void Loader::load_keyframe(const std::string &line, std::vector<keyframe>& keyframes)
{
    std::string l = line;
    l.erase(0, 10);
    std::vector<std::string> s = util::split_string(l, ' ');

    keyframe k;
    k.poses = new std::map<std::string, joint_transform>;
    k.time_stamp = stof(s[0]);

    std::vector<std::string> j = util::split_string(l, 'j');

    for (int i = 1; i < j.size(); i++)
    {
        load_joint(j[i], k.poses);
    }

    keyframes.push_back(k);


}
