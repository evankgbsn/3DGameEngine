#version 460 core

in vec4 clipSpace;
in vec2 uv;
in vec3 toCamera;

layout(binding = 0) uniform sampler2D reflectionTexture;
layout(binding = 1) uniform sampler2D refractionTexture;
layout(binding = 2) uniform sampler2D DuDv;

layout(std140, binding = 1) uniform EffectsUBO {
	vec4 tint;
	float tintStrength;
    float moveFactor;
	float waveStrength;
	float reflectionStrength;
} effectsUBO;

out vec4 color;

void main(void)
{
	vec2 ndc = (clipSpace.xy/clipSpace.w) / 2.0f + 0.5f;
	vec2 refrUV = ndc;
	vec2 reflUV = vec2(ndc.x, -ndc.y);

	vec2 distortion1 = (texture(DuDv, vec2(uv.x + effectsUBO.moveFactor, uv.y)).rg * 2.0f - 1.0f) * effectsUBO.waveStrength;
	vec2 distortion2 = (texture(DuDv, vec2(-uv.x + effectsUBO.moveFactor, uv.y + effectsUBO.moveFactor)).rg * 2.0f - 1.0f) * effectsUBO.waveStrength;
	vec2 totalDistortion = distortion1 + distortion2;

	refrUV += totalDistortion;
	reflUV += totalDistortion;

	refrUV = clamp(refrUV, 0.001f, 0.999f);
	reflUV.x = clamp(reflUV.x, 0.001f, 0.999f);
	reflUV.y = clamp(reflUV.y, -0.999f, -0.001f);

	vec4 reflColor = texture(reflectionTexture, reflUV);
	vec4 refrColor = texture(refractionTexture, refrUV);

	vec3 viewVector = normalize(toCamera);

	float refractiveFactor = dot(viewVector, vec3(0.0f, 1.0f, 0.0f));
	refractiveFactor = pow(refractiveFactor, effectsUBO.reflectionStrength);

	//reflColor = mix(reflColor, vec4(0.0f, 0.0f, 0.0f, 1.0f), effectsUBO.reflectionStrength);

	color = mix(reflColor, refrColor, refractiveFactor);
	color = mix(color, effectsUBO.tint, effectsUBO.tintStrength);
}

