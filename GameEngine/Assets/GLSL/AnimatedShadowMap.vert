#version 460 core

layout(std140, binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(std140, binding = 1) uniform AnimData
{
    mat4 pose[500];
    mat4 invBindPose[500];
} anim;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;

void main(void)
{
     mat4 skin = (anim.pose[inJoints.x] * anim.invBindPose[inJoints.x]) * inWeights.x;
     skin += (anim.pose[inJoints.y] * anim.invBindPose[inJoints.y]) * inWeights.y;
     skin += (anim.pose[inJoints.z] * anim.invBindPose[inJoints.z]) * inWeights.z;
     skin += (anim.pose[inJoints.w] * anim.invBindPose[inJoints.w]) * inWeights.w;

     gl_Position = mvp.projection * mvp.view * mvp.model * skin * vec4(inPosition, 1.0);
}