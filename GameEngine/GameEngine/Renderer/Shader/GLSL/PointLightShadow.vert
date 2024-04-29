#version 460 core

layout(std140, binding = 0) uniform Model {
    mat4 model;
} m;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;

void main(void)
{
    gl_Position = m.model * vec4(inPosition, 1.0f);
}