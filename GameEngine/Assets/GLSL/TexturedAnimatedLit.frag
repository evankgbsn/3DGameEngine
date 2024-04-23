#version 460 core

//--------------------------------------------------
// Uniform buffer
//--------------------------------------------------

layout(std140, binding = 2) uniform DirectionalLightUBO
{
	vec4 color;
	vec4 direction;
} directionalLight;

layout(std140, binding = 3) uniform AmbientUBO
{
	vec4 color;
} ambient;

layout(std140, binding = 4) uniform PointLightUBO
{
	vec4 color;
	vec4 position;
	float constant;
	float linear;
	float quadratic;
} pointLight;

layout(std140, binding = 5) uniform SpotLight
{
	vec4 color;
	vec4 position;
	vec4 direction;
	float cutoff;
	float outerCutoff;
	float constant;
	float linear;
	float quadratic;
} spotLight;

layout(std140, binding = 6) uniform ViewPositionUBO
{
	vec4 position;
} viewPosition;

layout(std140, binding = 7) uniform MaterialUBO
{
	float shine;
} material;


//--------------------------------------------------
// Image Texture Data Samplers
//--------------------------------------------------

layout(binding = 0) uniform sampler2D diffuseSampler;
layout(binding = 1) uniform sampler2D specularSampler;

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
	// Ambient Light
    vec4 ambientLightColor = ambient.color * texture(diffuseSampler, inUV);

	// Directional Light
	float diff = max(dot(inNormal.xyz, normalize(-directionalLight.direction.xyz)), 0.0f);
	vec4 diffuseLight = directionalLight.color * (diff * texture(diffuseSampler, inUV));

	vec3 viewDir = normalize(viewPosition.position - inPosition).xyz;
	vec3 reflectDir = reflect(directionalLight.direction.xyz, inNormal.xyz);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
	vec3 specularLight = texture(specularSampler, inUV).xyz * spec * directionalLight.color.xyz;

	vec4 directionalLightColor = (diffuseLight + vec4(specularLight, 1.0f));

	// Point Light
	vec3 normal = normalize(pointLight.position - inPosition).xyz;
	diff = max(dot(inNormal.xyz, normal), 0.0f);
	diffuseLight = pointLight.color * (diff * texture(diffuseSampler, inUV));

	viewDir = normalize(viewPosition.position - inPosition).xyz;
	reflectDir = reflect(-normal, inNormal.xyz);
	spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
	specularLight = texture(specularSampler, inUV).xyz * spec * pointLight.color.xyz;

	float distance = length(pointLight.position - inPosition);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));  

	diffuseLight *= attenuation;
	specularLight *= attenuation;

	vec4 pointLightColor = (diffuseLight + vec4(specularLight, 1.0f));

	// Spot Light
	float theta = dot(normalize(spotLight.position - inPosition), -spotLight.direction);
	float epsilon = spotLight.cutoff - spotLight.outerCutoff;
	float fadeIntensity = clamp((theta - spotLight.outerCutoff)/ epsilon, 0.0f, 1.0f);
	
	if (theta > spotLight.outerCutoff)
	{
		normal = normalize(spotLight.position - inPosition).xyz;
		diff = max(dot(inNormal.xyz, normal), 0.0f);
		diffuseLight = spotLight.color * (diff * texture(diffuseSampler, inUV));

		viewDir = normalize(viewPosition.position - inPosition).xyz;
		reflectDir = reflect(-normal, inNormal.xyz);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);
		specularLight = texture(specularSampler, inUV).xyz * spec * spotLight.color.xyz;

		distance = length(spotLight.position - inPosition);
		attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));  

		diffuseLight *= attenuation;
		specularLight *= attenuation;

		diffuseLight *= fadeIntensity;
		specularLight *= fadeIntensity;

	}

	vec4 spotLightColor = (diffuseLight + vec4(specularLight, 1.0f));
	
	color = directionalLightColor + pointLightColor + spotLightColor + ambientLightColor;
};


