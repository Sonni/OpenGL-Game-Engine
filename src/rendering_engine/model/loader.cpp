#include "loader.h"


void Loader::LoadBones(const std::string& line, std::vector<Bone>& boneRef, int uniqueId)
{
    std::string bones = line;
    bones.erase(0, 6);
    std::vector<std::string> boneData = Split(bones, '/');
    
    Bone bone;
    bone.m_id = boneData[0];
    bone.m_parentId = boneData[1];
    
    if(boneData.size() > 3) //Has children
    {
        std::vector<std::string> children;
        for(int i = 2; i < boneData.size() - 1; i++)
            children.push_back(boneData[i]);
        
        bone.m_children = children;
    }

    std::vector<std::string> mData = Split(boneData[boneData.size() - 1], ' ');
    
    mat4f m;
    m[0][0] = stof(mData[0]); m[1][0] = stof(mData[1]); m[2][0] = stof(mData[2]); m[3][0] = stof(mData[3]);
    m[0][1] = stof(mData[4]); m[1][1] = stof(mData[5]); m[2][1] = stof(mData[6]); m[3][1] = stof(mData[7]);
    m[0][2] = stof(mData[8]); m[1][2] = stof(mData[9]); m[2][2] = stof(mData[10]); m[3][2] = stof(mData[11]);
    m[0][3] = stof(mData[12]); m[1][3] = stof(mData[13]); m[2][3] = stof(mData[14]); m[3][3] = stof(mData[15]);
    
    bone.m_jointMatrix = m;
    bone.m_uniqueId = uniqueId;
    
    boneRef.push_back(bone);
    
}

void Loader::LoadBoneArray(const std::string& line, std::vector<std::string>& boneIdsRef)
{
    std::string boneIds = line;
    boneIds.erase(0, 4);
    
    std::vector<std::string> idArray = Split(boneIds, ' ');
    for(int i = 0; i < idArray.size(); i++)
        boneIdsRef.push_back(idArray[i]);
}

void Loader::LoadInvBindMatrix(const std::string& line, std::vector<Bone>& boneRef, std::vector<std::string>& boneIdsRef)
{
    std::string mat = line;
    mat.erase(0, 9);
    
    std::vector<std::string> mData = Split(mat, ' ');
    std::vector<mat4f> m;
    for(int i = 0; i < mData.size() / 16; i++)
    {
        m.push_back(mat4f().init_identity());
        m[i][0][0] = stof(mData[0+i*16]); m[i][1][0] = stof(mData[1+i*16]); m[i][2][0] = stof(mData[2+i*16]); m[i][3][0] = stof(mData[3+i*16]);
        m[i][0][1] = stof(mData[4+i*16]); m[i][1][1] = stof(mData[5+i*16]); m[i][2][1] = stof(mData[6+i*16]); m[i][3][1] = stof(mData[7+i*16]);
        m[i][0][2] = stof(mData[8+i*16]); m[i][1][2] = stof(mData[9+i*16]); m[i][2][2] = stof(mData[10+i*16]); m[i][3][2] = stof(mData[11+i*16]);
        m[i][0][3] = stof(mData[12+i*16]); m[i][1][3] = stof(mData[13+i*16]); m[i][2][3] = stof(mData[14+i*16]); m[i][3][3] = stof(mData[15+i*16]);
    }
    
    for(int i = 0; i < m.size(); i++)
        for(int j = 0; j < boneRef.size(); j++)
            if(boneIdsRef[i] == boneRef[j].m_id)
                boneRef[j].m_invBindMatrix = m[i];
    
}

void Loader::LoadWeights(const std::string& line, std::vector<float>& weightsRef)
{
    std::string weight = line;
    weight.erase(0, 3);
    
    std::vector<std::string> weightArray = Split(weight, ' ');
    for(int i = 0; i < weightArray.size(); i++)
        weightsRef.push_back(stof(weightArray[i]));
}

void Loader::LoadJCount(const std::string& line, std::vector<int>& jCountRef)
{
    std::string jCount = line;
    jCount.erase(0, 8);
    
    std::vector<std::string> jCountArray = Split(jCount, ' ');
    for(int i = 0; i < jCountArray.size(); i++)
        jCountRef.push_back(stoi(jCountArray[i]));
}


void Loader::LoadJs(const std::string& line, std::vector<int>& jsRef)
{
    std::string j = line;
    j.erase(0, 3);
    
    std::vector<std::string> jArray = Split(j, ' ');
    for(int i = 0; i < jArray.size(); i++)
        jsRef.push_back(stoi(jArray[i]));
}

void Loader::LoadBindShape(const std::string& line, mat4f& ma)
{
    std::string mat = line;
    mat.erase(0, 11);
    
    std::vector<std::string> mData = Split(mat, ' ');
   
    mat4f m;
    m[0][0] = stof(mData[0]); m[1][0] = stof(mData[1]); m[2][0] = stof(mData[2]); m[3][0] = stof(mData[3]);
    m[0][1] = stof(mData[4]); m[1][1] = stof(mData[5]); m[2][1] = stof(mData[6]); m[3][1] = stof(mData[7]);
    m[0][2] = stof(mData[8]); m[1][2] = stof(mData[9]); m[2][2] = stof(mData[10]); m[3][2] = stof(mData[11]);
    m[0][3] = stof(mData[12]); m[1][3] = stof(mData[13]); m[2][3] = stof(mData[14]); m[3][3] = stof(mData[15]);
    
    ma = m;
}

void Loader::LoadTime(const std::string& line, std::vector<float>& timeRef, std::string& nameRef)
{
    std::string time = line;
    time.erase(0, 3);
    
    std::vector<std::string> nameAndTime = Split(time, '/');
    nameRef = nameAndTime[0];
    
    std::vector<std::string> timeArray = Split(nameAndTime[1], ' ');
    for(int i = 0; i < timeArray.size(); i++)
        timeRef.push_back(stof(timeArray[i]));
}

void Loader::LoadAnimation(const std::string& line, std::vector<mat4f>& matRef)
{
    std::string mat = line;
    mat.erase(0, 3);
    
    std::vector<std::string> mData = Split(mat, ' ');
    
    for(int i = 0; i < mData.size() / 16; i++)
    {
        mat4f m;
        m[0][0] = stof(mData[0+i*16]); m[1][0] = stof(mData[1+i*16]); m[2][0] = stof(mData[2+i*16]); m[3][0] = stof(mData[3+i*16]);
        m[0][1] = stof(mData[4+i*16]); m[1][1] = stof(mData[5+i*16]); m[2][1] = stof(mData[6+i*16]); m[3][1] = stof(mData[7+i*16]);
        m[0][2] = stof(mData[8+i*16]); m[1][2] = stof(mData[9+i*16]); m[2][2] = stof(mData[10+i*16]); m[3][2] = stof(mData[11+i*16]);
        m[0][3] = stof(mData[12+i*16]); m[1][3] = stof(mData[13+i*16]); m[2][3] = stof(mData[14+i*16]); m[3][3] = stof(mData[15+i*16]);
        
        matRef.push_back(m);
    }
}
