#version 460 core

//--------------------------------------------------
// Model, View, Projection Uniform Buffer Object
//--------------------------------------------------

layout(std140, binding = 0) uniform MVP 
{
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(binding = 10) uniform ClipPlaneUBO
{
    vec4 plane;
} clipPlaneUBO;

//--------------------------------------------------
// Animation Data Uniform Buffer Object
//--------------------------------------------------

layout(std140, binding = 1) uniform AnimData
{
    mat4 pose[500];
} anim;

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
layout(location = 5) in vec4 inTangent;

//--------------------------------------------------
// Data Sent to Fragment Shader
//--------------------------------------------------

layout(location = 6) out vec2 outUV;
layout(location = 7) out vec4 outNormal;
layout(location = 8) out vec4 outPosition;
layout(location = 9) out vec4 outLightSpacePosition;
layout(location = 10) out mat3 outTBN;

void main(void) 
{
     mat4 skin = anim.pose[inJoints.x] * inWeights.x;
     skin += anim.pose[inJoints.y] * inWeights.y;
     skin += anim.pose[inJoints.z] * inWeights.z;
     skin += anim.pose[inJoints.w] * inWeights.w;

     vec4 worldPosition = mvp.model * skin * vec4(inPosition, 1.0);

     gl_ClipDistance[0] = dot(worldPosition, clipPlaneUBO.plane);

     gl_Position = mvp.projection * mvp.view * worldPosition;

     // Transform Normal and Tangent to World Space
     vec3 T = normalize(vec3(mvp.model * skin * vec4(inTangent.xyz, 0.0f)));
     vec3 N = normalize(vec3(mvp.model * skin * vec4(inNormal, 0.0f)));

     // Re-orthogonalize T with respect to N
     T = normalize(T - dot(T, N) * N);
     // Calculate Bitangent 
     vec3 B = cross(N, T) * inTangent.w;
    
     outTBN = mat3(T, B, N); // The matrix to transform Tangent Space to World Space

     outUV = inUV;
     outNormal = mvp.model * skin * vec4(inNormal, 0.0f);
     outPosition = worldPosition;
     outLightSpacePosition = lightSpaceMatrix.matrix * worldPosition;
}