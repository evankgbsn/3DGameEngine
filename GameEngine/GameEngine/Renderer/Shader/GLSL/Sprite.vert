#version 460 core

//--------------------------------------------------
// Model, View, Projection Uniform Buffer Object
//--------------------------------------------------

layout(std140, binding = 0) uniform Projection {
    mat4 proj;
} projection;

layout(std140, binding = 2) uniform Model {
    mat4 model;
} modelMat;


//--------------------------------------------------
// Vertex Buffer Data
//--------------------------------------------------

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;

//--------------------------------------------------
// Out to fragment shader
//--------------------------------------------------

out vec2 texCoords;

void main()
{
    gl_Position = projection.proj * modelMat.model * vec4(inPosition, 1.0f);
    texCoords = inUV;
}