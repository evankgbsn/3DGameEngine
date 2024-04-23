#include "GOLit.h"

#include "../Light//DirectionalLight.h"
#include "../Light/PointLight.h"
#include  "../Light/SpotLight.h"
#include "../Light/LightManager.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"
#include "../Texture/Texture.h"

#include <GL/glew.h>



GOLit::GOLit(Texture* const diffueseMap, Texture* const specularMap) :
	directionalLight(),
	pointLight(),
	ambient(),
	material(),
	viewPosition(),
	directionalLightBuffer(),
	pointLightBuffer(),
	ambientBuffer(),
	viewPositionBuffer(),
	materialBuffer(),
	specular(specularMap),
	diffuse(diffueseMap)
{
	glCreateBuffers(1, &directionalLightBuffer);
	glNamedBufferStorage(directionalLightBuffer, sizeof(DirectionalLightUBO), &directionalLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &ambientBuffer);
	glNamedBufferStorage(ambientBuffer, sizeof(AmbientUBO), &ambient, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &pointLightBuffer);
	glNamedBufferStorage(pointLightBuffer, sizeof(PointLightUBO), &pointLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &spotLightBuffer);
	glNamedBufferStorage(spotLightBuffer, sizeof(SpotLightUBO), &spotLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &viewPositionBuffer);
	glNamedBufferStorage(viewPositionBuffer, sizeof(ViewPositionUBO), &viewPosition, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &viewPositionBuffer);
	glNamedBufferStorage(viewPositionBuffer, sizeof(ViewPositionUBO), &viewPosition, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &materialBuffer);
	glNamedBufferStorage(materialBuffer, sizeof(MaterialUBO), &material, GL_DYNAMIC_STORAGE_BIT);
}

GOLit::~GOLit()
{
	glDeleteBuffers(1, &materialBuffer);
	glDeleteBuffers(1, &viewPositionBuffer);
	glDeleteBuffers(1, &spotLightBuffer);
	glDeleteBuffers(1, &pointLightBuffer);
	glDeleteBuffers(1, &ambientBuffer);
	glDeleteBuffers(1, &directionalLightBuffer);
}

void GOLit::UpdateLighting()
{
	diffuse->Bind(GL_TEXTURE0);
	specular->Bind(GL_TEXTURE1);

	DirectionalLight* dLight = LightManager::GetDirectionalLightsAtIndices({ 0 })[0];
	directionalLight.direction = dLight->GetDirection();
	directionalLight.color = dLight->GetColor();
	ambient.ambient = glm::vec4(1.0f) * LightManager::GetAmbientIntensity();

	PointLight* pLight = LightManager::GetPointLightsAtIndices({ 0 })[0];
	pointLight.position = glm::vec4(pLight->GetPosition(), 1.0f);
	pointLight.color = pLight->GetColor();
	pointLight.constant = pLight->GetConstant();
	pointLight.linear = pLight->GetLinear();
	pointLight.quadratic = pLight->GetQuadratic();

	SpotLight* sLight = LightManager::GetSpotLightsAtIndices({ 0 })[0];
	spotLight.color = sLight->GetColor();
	spotLight.position = glm::vec4(sLight->GetPosition(), 1.0f);
	spotLight.direction = glm::vec4(sLight->GetDirection(), 0.0f);
	spotLight.cutoff = sLight->GetCutoff();
	spotLight.constant = sLight->GetConstant();
	spotLight.linear = sLight->GetLinear();
	spotLight.quadratic = sLight->GetQudratic();
	spotLight.outerCutoff = sLight->GetOuterCuttoff();

	glm::vec4 viewPosition = glm::vec4(CameraManager::GetActiveCamera().GetPosition(), 1.0f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, directionalLightBuffer);
	glNamedBufferSubData(directionalLightBuffer, 0, sizeof(DirectionalLightUBO), &directionalLight);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ambientBuffer);
	glNamedBufferSubData(ambientBuffer, 0, sizeof(AmbientUBO), &ambient);

	glBindBufferBase(GL_UNIFORM_BUFFER, 4, pointLightBuffer);
	glNamedBufferSubData(pointLightBuffer, 0, sizeof(PointLightUBO), &pointLight);

	glBindBufferBase(GL_UNIFORM_BUFFER, 5, spotLightBuffer);
	glNamedBufferSubData(spotLightBuffer, 0, sizeof(SpotLightUBO), &spotLight);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 6, viewPositionBuffer);
	glNamedBufferSubData(viewPositionBuffer, 0, sizeof(glm::vec4), &viewPosition);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 7, materialBuffer);
	glNamedBufferSubData(materialBuffer, 0, sizeof(MaterialUBO), &material);

}

void GOLit::SetShine(float shine)
{
	material.shine = shine;
}
