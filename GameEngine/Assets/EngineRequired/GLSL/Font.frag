#version 460 core

in vec2 texCoords;
out vec4 color;

layout(std140, binding = 1) uniform ColorData
{
    vec4 c;
} colorData;

layout(binding = 0) uniform sampler2D sampler;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler, texCoords).r);
    color = colorData.c * sampled;
}  