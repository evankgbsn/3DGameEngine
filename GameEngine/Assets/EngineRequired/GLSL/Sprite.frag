#version 460 core

in vec2 texCoords;
out vec4 color;

layout(binding = 0) uniform sampler2D sampler;

void main()
{    
    color = texture(sampler, texCoords);
}  