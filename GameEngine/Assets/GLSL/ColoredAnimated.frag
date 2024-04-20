#version 460

//--------------------------------------------------
// Color Uniform Buffer Object
//--------------------------------------------------

layout(binding = 3) uniform ColorUBO {
    vec4 color;
} colorUBO;

//--------------------------------------------------
// Data Sent from Vertex Shader
//--------------------------------------------------



//--------------------------------------------------
// Final Color Sent to Rasterizer
//--------------------------------------------------

out vec4 color;

void main(void)
{
	color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}