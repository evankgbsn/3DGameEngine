#version 460 core

//--------------------------------------------------
// Uniform buffer
//--------------------------------------------------

layout(std140, binding = 2) uniform DirectionalLightUBO
{
	vec4 color;
	vec4 direction;
} directionalLight;

layout(std140, binding = 3) uniform AmbientStrengthUBO
{
	float strength;
} ambientStrength;

layout(std140, binding = 4) uniform PointLightUBO
{
	vec4 color;
	vec4 position;
} pointLight;

layout(std140, binding = 5) uniform ViewPositionUBO
{
	vec4 position;
} viewPosition;

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

	float specularStr = 0.5f;
	vec3 viewDir = normalize(viewPosition.position - inPosition).xyz;
	vec3 reflectDir = reflect(-normal, inNormal.xyz);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStr * spec * directionalLight.color.xyz;

	color = texture(sampler, inUV) * (lightIntensity + ambient + vec4(specular, 1.0f));
};

