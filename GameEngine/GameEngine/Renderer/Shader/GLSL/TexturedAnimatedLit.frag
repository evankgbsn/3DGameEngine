#version 460 core

//--------------------------------------------------
// Uniform buffer
//--------------------------------------------------

struct DirectionalLight
{
	vec4 color;
	vec4 direction;
	bool lightOn;
};

struct Ambient
{
	vec4 color;
};

struct PointLight
{
	vec4 color;
	vec4 position;
	float constant;
	float linear;
	float quadratic;
	bool lightOn;
};

struct SpotLight
{
	vec4 color;
	vec4 position;
	vec4 direction;
	float cutoff;
	float outerCutoff;
	float constant;
	float linear;
	float quadratic;
	bool lightOn;
};

struct Material
{
	float shine;
};

struct ViewPosition
{
	vec4 position;
};

layout(std140, binding = 2) uniform DirectionalLightUBO { DirectionalLight light[20]; } directionalLight;

layout(std140, binding = 3) uniform AmbientUBO { Ambient light; } ambient;

layout(std140, binding = 4) uniform PointLightUBO { PointLight light[20]; } pointLight;

layout(std140, binding = 5) uniform SpotLightUBO { SpotLight light[20]; } spotLight;

layout(std140, binding = 6) uniform ViewPositionUBO { ViewPosition pos; } viewPosition;

layout(std140, binding = 7) uniform MaterialUBO { Material mat; } material;

layout(std140, binding = 11) uniform FogUBO { float fogDensity; float fogGradient; } fog;


//--------------------------------------------------
// Image Texture Data Samplers
//--------------------------------------------------

layout(binding = 0) uniform sampler2D diffuseSampler;
layout(binding = 1) uniform sampler2D specularSampler;
layout(binding = 31) uniform sampler2D shadowMap;

//--------------------------------------------------
// Data Sent from Vertex Shader
//--------------------------------------------------

layout(location = 5) in vec2 inUV;
layout(location = 6) in vec4 inNormal;
layout(location = 7) in vec4 inPosition;
layout(location = 8) in vec4 inLightSpacePosition;

//--------------------------------------------------
// Final Color Sent to Rasterizer
//--------------------------------------------------

out vec4 color;

vec4 CalcAmbientLight(Ambient light);
vec4 CalcDirectionalLight(DirectionalLight light);
vec4 CalcPointLight(PointLight light);
vec4 CalcSpotLight(SpotLight light);
float CalcShadow();

void main(void)
{
	for(int i = 0; i < 20; ++i)
	{
		if(directionalLight.light[i].lightOn)
		{
			color += CalcDirectionalLight(directionalLight.light[i]);
		}

		if(pointLight.light[i].lightOn)
		{
			color += CalcPointLight(pointLight.light[i]);
		}

		if(spotLight.light[i].lightOn)
		{
			color += CalcSpotLight(spotLight.light[i]);
		}
	}

	color += CalcAmbientLight(ambient.light);

	float distance = length(viewPosition.pos.position.xyz - inPosition.xyz);
	float visibility = exp(-pow((distance * fog.fogDensity), fog.fogGradient));
	visibility = clamp(visibility, 0.0f, 1.0f);

	color = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), color, visibility);
};

vec4 CalcAmbientLight(Ambient light)
{
	return light.color * texture(diffuseSampler, inUV);
}

vec4 CalcDirectionalLight(DirectionalLight light)
{
	float diff = max(dot(inNormal.xyz, normalize(-light.direction.xyz)), 0.0f);
	vec4 diffuseLight = light.color * (diff * texture(diffuseSampler, inUV));

	vec3 viewDir = normalize(viewPosition.pos.position - inPosition).xyz;
	vec3 reflectDir = reflect(light.direction.xyz, inNormal.xyz);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.mat.shine);
	vec3 specularLight = texture(specularSampler, inUV).xyz * spec * light.color.xyz;

	return (1.0f - CalcShadow()) * (diffuseLight + vec4(specularLight, 1.0f));
}

vec4 CalcPointLight(PointLight light)
{
	// Point Light
	vec3 normal = normalize(light.position - inPosition).xyz;
	float diff = max(dot(inNormal.xyz, normal), 0.0f);
	vec4 diffuseLight = light.color * (diff * texture(diffuseSampler, inUV));

	vec3 lightDir = normalize(light.position - inPosition).xyz;
	vec3 viewDir = normalize(viewPosition.pos.position - inPosition).xyz;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(inNormal.xyz, halfwayDir), 0.0), material.mat.shine);
	vec3 specularLight = texture(specularSampler, inUV).xyz * spec * light.color.xyz;

	float distance = length(light.position - inPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  

	diffuseLight *= attenuation;
	specularLight *= attenuation;

	return diffuseLight + vec4(specularLight, 1.0f);
}

vec4 CalcSpotLight(SpotLight light)
{
	float theta = dot(normalize(light.position - inPosition), -light.direction);
	float epsilon = light.cutoff - light.outerCutoff;
	float fadeIntensity = clamp((theta - light.outerCutoff)/ epsilon, 0.0f, 1.0f);
	
	vec4 lightColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if (theta > light.outerCutoff)
	{
		vec3 normal = normalize(light.position - inPosition).xyz;
		float diff = max(dot(inNormal.xyz, normalize(light.position - inPosition).xyz), 0.0f);
		vec4 diffuseLight = light.color * (diff * texture(diffuseSampler, inUV));

		vec3 lightDir = normalize(light.position - inPosition).xyz;
		vec3 viewDir = normalize(viewPosition.pos.position - inPosition).xyz;
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(inNormal.xyz, halfwayDir), 0.0), material.mat.shine);
		vec3 specularLight = texture(specularSampler, inUV).xyz * spec * light.color.xyz;

		float distance = length(light.position - inPosition);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  

		diffuseLight *= attenuation;
		specularLight *= attenuation;

		diffuseLight *= fadeIntensity;
		specularLight *= fadeIntensity;
		
		lightColor = (diffuseLight + vec4(specularLight, 1.0f));
	}

	return lightColor;
}

float CalcShadow()
{
	// perform perspective divide
	vec3 projCoords = inLightSpacePosition.xyz / inLightSpacePosition.w;
	
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	if(projCoords.z > 1.0)
	{
		return 0;
	}
	
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// check whether current frag pos is in shadow
	float bias = 0.005f; //max(0.05 * (1.0 - dot(inNormal, directionalLight.light[0].direction)), 0.005); 
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;



	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
	        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
	    }    
	}
	shadow /= 9.0;
	return shadow;

}