#version 460 core

layout(std140, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 projection;
} vp;

layout(std140, binding = 8) uniform LightSpcaeMatrixUBO
{
    mat4 matrix;
} lightSpaceMatrix;

layout(binding = 10) uniform ClipPlaneUBO
{
    vec4 plane;
} clipPlaneUBO;

//--------------------------------------------------
// Vertex Buffer Data
//--------------------------------------------------

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;
layout(location = 5) in vec4 translation;
layout(location = 6) in vec4 right;
layout(location = 7) in vec4 up;
layout(location = 8) in vec4 forward;
layout(location = 9) in vec4 color;
layout(location = 10) in vec4 inTangent;

//--------------------------------------------------
// Data Sent to Fragment Shader
//--------------------------------------------------

layout(location = 11) out vec2 outUV;
layout(location = 12) out vec4 outNormal;
layout(location = 13) out vec4 outPosition;
layout(location = 14) out vec4 outLightSpacePosition;
layout(location = 15) out mat3 outTBN;

void main(void)
{
    mat4 transformMatrix = mat4(right, up, forward, translation);

    vec4 worldPosition = transformMatrix * vec4(inPosition, 1.0f);

    gl_ClipDistance[0] = dot(worldPosition, clipPlaneUBO.plane);

	gl_Position = vp.projection * vp.view * worldPosition;

    // Transform Normal and Tangent to World Space
    vec3 T = normalize(vec3(mvp.model * vec4(inTangent.xyz, 0.0f)));
    vec3 N = normalize(vec3(mvp.model * vec4(inNormal, 0.0f)));

    // Re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // Calculate Bitangent 
    vec3 B = cross(N, T) * inTangent.w;
    
    outTBN = mat3(T, B, N); // The matrix to transform Tangent Space to World Space

    outUV = inUV;

    vec4 normal =  transformMatrix * vec4(inNormal, 0.0f);
    outNormal = normal;
    outPosition = worldPosition;
    outLightSpacePosition = lightSpaceMatrix.matrix * worldPosition;
}

