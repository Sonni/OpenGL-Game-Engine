#ifndef ___D_rendering_engine__tennaLoader__
#define ___D_rendering_engine__tennaLoader__

#include "indexed_model.h"
#include "loader.h"

class TNAModel
{
public:
    std::vector<TNAIndex> m_TNAIndices;
    std::vector<vec3f> m_vertices;
    std::vector<vec2f> m_uvs;
    std::vector<vec3f> m_normals;
    
    Bone m_root;
    mat4f m_bindShapeMatrix;
    std::vector<Bone> m_bones;
    std::vector<std::string> m_boneIds;
    
    std::vector<float> m_weights;
    std::vector<int> m_jCount;
    std::vector<int> m_js;
    int m_idPointer = 0;
    
    std::vector<Skinning> m_skinning;
    std::vector<AnimationData> m_animation;
    
    
    TNAModel(const std::string& fileName);
    
    void CalcBonesWorldMatrix(int frame = -1);
    inline std::string GetFileName() const { return m_fileName; }

    bool has_animation = false;
    
    indexed_model to_indexed_model();
private:
    void LoadVertices(const std::string& line);
    void LoadUvs(const std::string& line);
    void LoadNormals(const std::string& line);
    void LoadFaces(const std::string& line);
    
    void SetupAnimation(float frames)
    {
        std::vector<mat4f> result;
        
        for(int i = 0; i < m_animation.size(); i++)
        {
            std::vector<mat4f> tmp;
            tmp.push_back(m_animation[i].m_transform[0]);
            for(int j = 0; j < m_animation[i].m_transform.size() - 1; j++)
            {
                std::vector<mat4f> tmpInter = Interpolate(m_animation[i].m_transform[j], m_animation[i].m_transform[j + 1], frames);
                for(int k = 0; k < tmpInter.size(); k++)
                    tmp.push_back(tmpInter[k]);
                tmp.push_back(m_animation[i].m_transform[j + 1]);
              
            }
            m_animation[i].m_transform = tmp;
        
        }
    }
    
    std::vector<mat4f> Interpolate(mat4f firstFrame, mat4f lastFrame, int frames)
    {
        std::vector<mat4f> result;
        
        for(int j = 0; j < frames; j++)
        {
            mat4f firstMatrix = firstFrame;
            mat4f lastMatrix = lastFrame;
            
            vec3f firstPos(firstMatrix[3][0], firstMatrix[3][1], firstMatrix[3][2]);
            vec3f lastPos(lastMatrix[3][0], lastMatrix[3][1], lastMatrix[3][2]);
            vec3f middelPos = (lastPos - firstPos) / 2 + firstPos;
            
            quaternion firstRot(firstMatrix);
            quaternion lastRot(lastMatrix);
            quaternion middelRot;
            middelRot = firstRot.s_lerp(lastRot, 1.0f / frames * j, true);
            
            mat4f middelTranslateMatrix;
            middelTranslateMatrix.init_translation(middelPos);
            
            mat4f middelRotMatrix;
            middelRotMatrix = middelRot.to_rotation_matrix();
           
            result.push_back(middelTranslateMatrix * middelRotMatrix);
            
        }

        return result;
    }
    
    void CalcChildWorldMat(const std::string& parentId, const std::string& childId, mat4f worldMat);
    
    int count = 0; //To get the root
    std::string m_fileName;
    
};

#endif