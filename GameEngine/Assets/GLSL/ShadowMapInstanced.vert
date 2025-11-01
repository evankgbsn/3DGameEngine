#version 460 core

layout(std140, binding = 0) uniform ViewProjection {
    mat4 view;
    mat4 projection;
} vp;

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

void main(void)
{
    mat4 transformMatrix = mat4(right, up, forward, translation);

    vec4 worldPosition = transformMatrix * vec4(inPosition, 1.0f);

	gl_Position = vp.projection * vp.view * worldPosition;
}