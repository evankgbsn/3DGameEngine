#version 460

//--------------------------------------------------
// Texture Sampler
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
}