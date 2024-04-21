#include "GOTexturedLit.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"

GOTexturedLit::GOTexturedLit(Model* const model, Texture* const tex) :
	GO3D(model),
	texture(tex),
	directionalLightBuffer(),
	directionalLight({glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(5.0f, 5.0f, 5.0f, 1.0f)}),
	ambientStrengthBuffer(),
	ambientStrength(0.3f)
{
	glCreateBuffers(1, &directionalLightBuffer);
	glNamedBufferStorage(directionalLightBuffer, sizeof(DirectionalLightUBO), &directionalLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &ambientStrengthBuffer);
	glNamedBufferStorage(ambientStrengthBuffer, sizeof(float), &ambientStrength, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &pointLightBuffer);
	glNamedBufferStorage(pointLightBuffer, sizeof(PointLightUBO), &pointLight, GL_DYNAMIC_STORAGE_BIT);
}

GOTexturedLit::~GOTexturedLit()
{
	glDeleteBuffers(1, &directionalLightBuffer);
	glDeleteBuffers(1, &ambientStrengthBuffer);
	glDeleteBuffers(1, &pointLightBuffer);
}

void GOTexturedLit::Update()
{
	ShaderManager::StartShaderUsage("TexturedLit");

	texture->Bind(GL_TEXTURE0);

	DirectionalLight* dLight = LightManager::GetDirectionalLightsAtIndices({ 0 })[0];
	directionalLight.direction = dLight->GetDirection();
	directionalLight.color = dLight->GetColor();
	ambientStrength = LightManager::GetAmbientIntensity();

	PointLight* pLight = LightManager::GetPointLightsAtIndices({ 0 })[0];
	pointLight.position = glm::vec4(pLight->GetPosition(), 1.0f);
	pointLight.color = pLight->GetColor();

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, directionalLightBuffer);
	glNamedBufferSubData(directionalLightBuffer, 0, sizeof(DirectionalLightUBO), &directionalLight);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, ambientStrengthBuffer);
	glNamedBufferSubData(ambientStrengthBuffer, 0, sizeof(float), &ambientStrength);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, pointLightBuffer);
	glNamedBufferSubData(pointLightBuffer, 0, sizeof(PointLightUBO), &pointLight);

	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedLit");
}
