#version 460 core

layout(location = 10) in vec4 fragColor;

out vec4 color;

void main(void)
{
	color = fragColor;
}

