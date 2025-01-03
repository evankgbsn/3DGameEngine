#version 460 core

//--------------------------------------------------
// Model, View, Projection Uniform Buffer Object
//--------------------------------------------------

layout(std140, binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(std140, binding = 8) uniform LightSpcaeMatrixUBO
{
    mat4 matrix;
} lightSpaceMatrix;

//--------------------------------------------------
// Vertex Buffer Data
//--------------------------------------------------

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;

//--------------------------------------------------
// Data Sent to Fragment Shader
//--------------------------------------------------

layout(location = 5) out vec2 outUV;
layout(location = 6) out vec4 outNormal;
layout(location = 7) out vec4 outPosition;
layout(location = 8) out vec4 outLightSpacePosition;

void main(void)
{
	gl_Position = mvp.projection * mvp.view * mvp.model * vec4(inPosition, 1.0f);

    outUV = inUV;

    vec4 normal =  mvp.model * vec4(inNormal, 0.0f);
    vec4 position = mvp.model * vec4(inPosition, 1.0f);
    outNormal = normal;
    outPosition = position;
    outLightSpacePosition = lightSpaceMatrix.matrix * outPosition;
}

