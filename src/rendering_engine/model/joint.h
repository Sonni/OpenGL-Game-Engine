//
// Created by Sonni on 25/08/2017.
//

#ifndef INC_3D_ENGINE_JOINT_H
#define INC_3D_ENGINE_JOINT_H


#include <vector>
#include "../../core_engine/math/matrix.h"

class joint
{
public:
    int id;
    std::string name;
    int parent_id;
    mat4f localBindTransform; // This is the actual inverse bind transform, just transposed
    mat4f animatedTransform;
    mat4f inverseBindTransform;
    std::vector<joint*> m_children;





void calcInverseBindTransform(mat4f parentBindTransform)
{

    inverseBindTransform = localBindTransform.transpose();


    mat4f bindTransform = parentBindTransform * localBindTransform.transpose();



        for (joint* child : m_children) {
            child->calcInverseBindTransform(bindTransform);
        }
    }

};


#endif //INC_3D_ENGINE_JOINT_H
