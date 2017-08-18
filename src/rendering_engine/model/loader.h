#ifndef ___D_rendering_engine__loader__
#define ___D_rendering_engine__loader__

#include "util.h"
#include "../../core_engine/math/matrix.h"
#include "../../core_engine/math/quaternion.h"
#include <string>
#include <vector>


struct TNAIndex
{
    unsigned int vertexIndex;
    unsigned int uvIndex;
    unsigned int normalIndex;
    
    bool operator<(const TNAIndex& r) const { return vertexIndex < r.vertexIndex; }
};

struct Bone
{
    std::string m_id;
    std::string m_parentId;
    mat4f m_jointMatrix;
    mat4f m_worldMatrix;
    mat4f m_invBindMatrix;
    mat4f m_skinningMatrix;
    std::vector<std::string> m_children;
    int m_uniqueId;
};

struct Skinning
{
    int m_id;
    std::vector<float> m_weights;
    std::vector<int> m_boneId;
};

struct AnimationData
{
    std::vector<float> m_time;
    std::vector<mat4f> m_transform;
    std::string m_bone;
};


class Loader
{
public:
    
    static void LoadBones(const std::string& line, std::vector<Bone>& boneRef, int uniqueId);
    static void LoadBoneArray(const std::string& line, std::vector<std::string>& boneIdsRef);
    static void LoadInvBindMatrix(const std::string& line, std::vector<Bone>& boneRef, std::vector<std::string>& boneIdsRef);
    static void LoadWeights(const std::string& line, std::vector<float>& weightReft);
    static void LoadJCount(const std::string& line, std::vector<int>& jCountRef);
    static void LoadJs(const std::string& line, std::vector<int>& jsRef);
    static void LoadBindShape(const std::string& line, mat4f& m);
    
    static void LoadTime(const std::string& line, std::vector<float>& timeRef, std::string& nameRef);
    static void LoadAnimation(const std::string& line, std::vector<mat4f>& m);

    static std::vector<std::string> Split(const std::string& s, char delim)
    {
        std::vector<std::string> elems;

        const char* cstr = s.c_str();
        unsigned int strLength = (unsigned int)s.length();
        unsigned int start = 0;
        unsigned int end = 0;

        while(end <= strLength)
        {
            while(end <= strLength)
            {
                if(cstr[end] == delim)
                    break;
                end++;
            }

            elems.push_back(s.substr(start, end - start));
            start = end + 1;
            end = start;
        }

        return elems;
    }


protected:
private:

};

#endif