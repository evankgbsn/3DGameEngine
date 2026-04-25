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
layout(binding = 2) uniform sampler2D normalSampler;
layout(binding = 31) uniform sampler2D shadowMap;

//--------------------------------------------------
// Data Sent from Vertex Shader
//--------------------------------------------------

layout(location = 6) in vec2 inUV;
layout(location = 7) in vec4 inNormal;
layout(location = 8) in vec4 inPosition;
layout(location = 9) in vec4 inLightSpacePosition;
layout(location = 10) in mat3 inTBN;

//--------------------------------------------------
// Final Color Sent to Rasterizer
//--------------------------------------------------

out vec4 color;

vec4 CalcAmbientLight(Ambient light);
vec4 CalcDirectionalLight(DirectionalLight light, vec4 normalMapNormal, float shadow);
vec4 CalcPointLight(PointLight light, vec4 normalMapNormal);
vec4 CalcSpotLight(SpotLight light, vec4 normalMapNormal);
float CalcShadow(vec4 normalMapNormal);
float CalcPointShadow();

void main(void)
{
	// 1. Sample the normal map 
    vec3 normal = texture(normalSampler, inUV).rgb;
    
    // 2. Transform from [0,1] to [-1,1] 
    normal = normalize(normal * 2.0 - 1.0); 
    
    // 3. Transform to World Space using TBN matrix 
    // Use this 'worldNormal' instead of 'inNormal' in all CalcLight functions
    vec3 worldNormal = normalize(inTBN * normal);

	float shadow = CalcShadow(vec4(worldNormal, 0.0f)); 

	for(int i = 0; i < 20; ++i)
	{
		if(directionalLight.light[i].lightOn) 
		{
			// Only apply the shadow to the first light (the one that generated the map)
			float s = (i == 0) ? shadow : 0.0; 
			color += CalcDirectionalLight(directionalLight.light[i], vec4(worldNormal, 0.0f), s);
		}

		if(pointLight.light[i].lightOn)
		{
			color += CalcPointLight(pointLight.light[i], vec4(worldNormal, 0.0f));
		}

		if(spotLight.light[i].lightOn)
		{
			color += CalcSpotLight(spotLight.light[i], vec4(worldNormal, 0.0f));
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

vec4 CalcDirectionalLight(DirectionalLight light, vec4 normalMapNormal, float shadow)
{
	float diff = max(dot(normalMapNormal.xyz, normalize(-light.direction.xyz)), 0.0f);
	vec4 diffuseLight = light.color * (diff * texture(diffuseSampler, inUV));

	vec3 viewDir = normalize(viewPosition.pos.position - inPosition).xyz;
	vec3 reflectDir = reflect(light.direction.xyz, normalMapNormal.xyz);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.mat.shine);
	vec3 specularLight = texture(specularSampler, inUV).xyz * spec * light.color.xyz;

	return (1.0f - shadow) * (diffuseLight + vec4(specularLight, 0.0f)); // Use 0.0 alpha here
}

vec4 CalcPointLight(PointLight light, vec4 normalMapNormal)
{
	// Point Light
	vec3 normal = normalize(light.position - inPosition).xyz;
	float diff = max(dot(normalMapNormal.xyz, normal), 0.0f);
	vec4 diffuseLight = light.color * (diff * texture(diffuseSampler, inUV));

	vec3 lightDir = normalize(light.position - inPosition).xyz;
	vec3 viewDir = normalize(viewPosition.pos.position - inPosition).xyz;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normalMapNormal.xyz, halfwayDir), 0.0), material.mat.shine);
	vec3 specularLight = texture(specularSampler, inUV).xyz * spec * light.color.xyz;

	float distance = length(light.position - inPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  

	diffuseLight *= attenuation;
	specularLight *= attenuation;

	return diffuseLight + vec4(specularLight, 1.0f);
}

vec4 CalcSpotLight(SpotLight light, vec4 normalMapNormal)
{
    vec3 lightDir = normalize(light.position.xyz - inPosition.xyz);
    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = light.cutoff - light.outerCutoff;
    float fadeIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);
    
    if (theta > light.outerCutoff)
    {
        float diff = max(dot(normalMapNormal.xyz, lightDir), 0.0f);
        vec4 diffuseLight = light.color * (diff * texture(diffuseSampler, inUV));

        vec3 viewDir = normalize(viewPosition.pos.position.xyz - inPosition.xyz);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normalMapNormal.xyz, halfwayDir), 0.0), material.mat.shine);
        vec3 specularLight = texture(specularSampler, inUV).xyz * spec * light.color.xyz;

        float distance = length(light.position.xyz - inPosition.xyz);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        diffuseLight *= attenuation * fadeIntensity;
        specularLight *= attenuation * fadeIntensity;
        
        return (diffuseLight + vec4(specularLight, 0.0f));
    }
    return vec4(0.0f, 0.0f, 0.0f, 1.0f); // Fallback return [cite: 193]
}

float CalcShadow(vec4 normalMapNormal)
{
    vec3 projCoords = inLightSpacePosition.xyz / inLightSpacePosition.w; [cite: 193]
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) return 0.0;
    
    float currentDepth = projCoords.z;
    float bias = max(0.0005 * (1.0 - dot(normalMapNormal.xyz, normalize(-directionalLight.light[0].direction.xyz))), 0.00005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) [cite: 193]
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    return shadow / 9.0;
}