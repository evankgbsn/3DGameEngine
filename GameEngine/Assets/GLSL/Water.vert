#version 460 core

layout(std140, binding = 0) uniform MVP {
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

layout(std140, binding = 2) uniform CameraUBO {
    vec4 camPosition;
} cameraUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inWeights;
layout(location = 4) in ivec4 inJoints;

out vec4 clipSpace;
out vec2 uv;
out vec3 toCamera;

const float tiling = 5.0f;

void main(void)
{
    vec4 worldPosition = mvp.model * vec4(inPosition, 1.0f);

	gl_Position = mvp.projection * mvp.view * worldPosition;

    clipSpace = gl_Position;
    uv = inUV * tiling;
    toCamera = cameraUBO.camPosition.xyz - worldPosition.xyz;
}

