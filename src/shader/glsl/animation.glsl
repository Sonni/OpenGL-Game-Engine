#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_textureCoords;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in ivec3 in_jointIndices;
layout(location = 4) in vec3 in_weights;

const int MAX_JOINTS = 50;//max joints allowed in a skeleton
const int MAX_WEIGHTS = 3;//max number of joints that can affect a vertex

out vec2 pass_textureCoords;

uniform mat4 jointTransforms[MAX_JOINTS];
uniform mat4 projectionViewMatrix;


void main()
{
   vec4 totalLocalPos = vec4(0.0);
   	vec4 totalNormal = vec4(0.0);

   	for(int i=0;i<MAX_WEIGHTS;i++){
   		mat4 jointTransform = jointTransforms[in_jointIndices[i]];
   		vec4 posePosition = jointTransform * vec4(in_position, 1.0);
   		totalLocalPos += posePosition * in_weights[i];

   		vec4 worldNormal = jointTransform * vec4(in_normal, 0.0);
   		totalNormal += worldNormal * in_weights[i];

   	}


   	gl_Position = projectionViewMatrix *  totalLocalPos;
   	pass_textureCoords = in_textureCoords;
}

//-END_OF_VS-
#version 330 core

in vec2 pass_textureCoords;

out vec4 out_colour;

uniform sampler2D diffuseMap;


void main()
{
    vec4 diffuseColour = texture(diffuseMap, pass_textureCoords);

    	out_colour = diffuseColour;
}