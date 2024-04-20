#version 460 core

layout(std140, binding = 1) uniform Color
{
	vec4 color;
} colorUniform;

out vec4 color;

void main(void)
{
	color = colorUniform.color;
}

