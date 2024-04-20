#version 460 core

//--------------------------------------------------
// Uniform buffer
//--------------------------------------------------

//--------------------------------------------------
// Image Texture Data Samplers
//--------------------------------------------------

layout(binding = 0) uniform sampler2D sampler;

//--------------------------------------------------
// Data Sent from Vertex Shader
//--------------------------------------------------

layout(location = 5) in vec2 inUV;

//--------------------------------------------------
// Final Color Sent to Rasterizer
//--------------------------------------------------

out vec4 color;

void main(void)
{
	color = texture(sampler, inUV);
};

