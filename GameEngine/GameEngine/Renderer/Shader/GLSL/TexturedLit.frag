#version 460 core

//--------------------------------------------------
// Uniform buffer
//--------------------------------------------------

layout(std140, binding = 1) uniform DirectionalLightUBO
{
	vec4 color;
	vec4 direction;
} directionalLight;

layout(std140, binding = 2) uniform AmbientStrengthUBO
{
	float strength;
} ambientStrength;

layout(std140, binding = 3) uniform PointLightUBO
{
	vec4 color;
	vec4 position;
} pointLight;

//--------------------------------------------------
// Image Texture Data Samplers
//--------------------------------------------------

layout(binding = 0) uniform sampler2D sampler;

//--------------------------------------------------
// Data Sent from Vertex Shader
//--------------------------------------------------

layout(location = 5) in vec2 inUV;
layout(location = 6) in vec4 inNormal;
layout(location = 7) in vec4 inPosition;

//--------------------------------------------------
// Final Color Sent to Rasterizer
//--------------------------------------------------

out vec4 color;

void main(void)
{
	float ambientStr = ambientStrength.strength;
    vec4 ambient = ambientStr * directionalLight.color;

	vec3 normal = normalize(pointLight.position - inPosition).xyz;

	float diffuse = max(dot(inNormal.xyz, normal), 0.0f);
	vec4 lightIntensity = directionalLight.color * diffuse;

	color = texture(sampler, inUV) * (lightIntensity + ambient);
};

