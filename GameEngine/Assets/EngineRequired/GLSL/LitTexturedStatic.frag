#version 460

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform LightsUniformBufferObject {
    vec4 ambient;
    vec4 direction;
    vec4 color;
} lightsUBO;

layout(std140, binding = 11) uniform FogUBO { float fogDensity; float fogGradient; } fog;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUVCoord;
layout(location = 2) in vec4 inPosition;
layout(location = 0) out vec4 outColor;

vec4 DirectionalLight()
{
    float diffuseFactor = max(dot(fragNormal, -lightsUBO.direction.xyz), 0.0f);
    vec4 sampledFragColor = texture(texSampler, fragUVCoord);

    vec4 ambient = vec4(lightsUBO.ambient.xyz * sampledFragColor.xyz, sampledFragColor.a);
    vec4 diffuse = vec4(lightsUBO.color.xyz * diffuseFactor, sampledFragColor.a);

    diffuse *= sampledFragColor;
    ambient *= sampledFragColor;

    return ambient + diffuse;
};

void main(void)
{
    float distance = length(viewPosition.pos.position.xyz - inPosition.xyz);
	float visibility = exp(-pow((distance * fog.fogDensity), fog.fogGradient));
	visibility = clamp(visibility, 0.0f, 1.0f);

	outColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), DirectionalLight(), visibility);
}
