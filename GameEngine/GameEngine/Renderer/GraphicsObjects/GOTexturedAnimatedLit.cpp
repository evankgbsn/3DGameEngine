#include "GOTexturedAnimatedLit.h"

#include "../Shader/ShaderManager.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"
#include "../Texture/Texture.h"
#include "../Model/Model.h"
#include "../Animation/Animation.h"
#include "../Animation/Armature.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"

#include <GL/glew.h>

GOTexturedAnimatedLit::GOTexturedAnimatedLit(Model* const model, Texture* const tex) :
	GO3DAnimated(model),
	texture(tex),
	directionalLight(),
	directionalLightBuffer(),
	pointLight(),
	pointLightBuffer(),
	ambientLight(),
	ambientLightBuffer()
{
	glCreateBuffers(1, &directionalLightBuffer);
	glNamedBufferStorage(directionalLightBuffer, sizeof(DirectionalLightUBO), &directionalLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &ambientLightBuffer);
	glNamedBufferStorage(ambientLightBuffer, sizeof(float), &ambientLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &pointLightBuffer);
	glNamedBufferStorage(pointLightBuffer, sizeof(PointLightUBO), &pointLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &viewPositionBuffer);
	glNamedBufferStorage(viewPositionBuffer, sizeof(glm::vec4), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

GOTexturedAnimatedLit::~GOTexturedAnimatedLit()
{
	glDeleteBuffers(1, &viewPositionBuffer);
	glDeleteBuffers(1, &pointLightBuffer);
	glDeleteBuffers(1, &directionalLightBuffer);
	glDeleteBuffers(1, &ambientLightBuffer);
}

void GOTexturedAnimatedLit::Update()
{
	ShaderManager::StartShaderUsage("TexturedAnimatedLit");

	texture->Bind(GL_TEXTURE0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, directionalLightBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ambientLightBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, pointLightBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, viewPositionBuffer);

	ambientLight = LightManager::GetAmbientIntensity();
	
	DirectionalLight* dLight = LightManager::GetDirectionalLightsAtIndices({ 0 })[0];
	directionalLight.direction = dLight->GetDirection();
	directionalLight.color = dLight->GetColor();

	PointLight* pLight = LightManager::GetPointLightsAtIndices({ 0 })[0];
	pointLight.position = glm::vec4(pLight->GetPosition(), 1.0f);
	pointLight.color = pLight->GetColor();

	glm::vec4 viewPosition = glm::vec4(CameraManager::GetActiveCamera().GetPosition(), 1.0f);

	glNamedBufferSubData(directionalLightBuffer, 0, sizeof(DirectionalLightUBO), &directionalLight);
	glNamedBufferSubData(ambientLightBuffer, 0, sizeof(float), &ambientLight);
	glNamedBufferSubData(pointLightBuffer, 0, sizeof(PointLightUBO), &pointLight);
	glNamedBufferSubData(viewPositionBuffer, 0, sizeof(glm::vec4), &viewPosition);

	GO3DAnimated::Update();
	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedAnimatedLit");
}
