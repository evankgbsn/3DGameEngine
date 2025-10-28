#include "GOLit.h"

#include "../Light//DirectionalLight.h"
#include "../Light/PointLight.h"
#include  "../Light/SpotLight.h"
#include "../Light/LightManager.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"
#include "../Texture/Texture.h"
#include "../GraphicsObjects/GO3D.h"
#include "../Renderer/Renderer.h"

#include <GL/glew.h>

GOLit::GOLit(const std::vector<Material>& mats) :
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
	materials(mats),
	fog({ 0.007f, 2.5f })
{
	glCreateBuffers(1, &directionalLightBuffer);
	glNamedBufferStorage(directionalLightBuffer, sizeof(directionalLight), &directionalLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &ambientBuffer);
	glNamedBufferStorage(ambientBuffer, sizeof(AmbientUBO), &ambient, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &pointLightBuffer);
	glNamedBufferStorage(pointLightBuffer, sizeof(pointLight), &pointLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &spotLightBuffer);
	glNamedBufferStorage(spotLightBuffer, sizeof(spotLight), &spotLight, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &viewPositionBuffer);
	glNamedBufferStorage(viewPositionBuffer, sizeof(ViewPositionUBO), &viewPosition, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &materialBuffer);
	glNamedBufferStorage(materialBuffer, sizeof(MaterialUBO), &material, GL_DYNAMIC_STORAGE_BIT);
	
	glCreateBuffers(1, &fogBuffer);
	glNamedBufferStorage(fogBuffer, sizeof(FogEffect), &fog, GL_DYNAMIC_STORAGE_BIT);
}

GOLit::~GOLit()
{
	glDeleteBuffers(1, &materialBuffer);
	glDeleteBuffers(1, &viewPositionBuffer);
	glDeleteBuffers(1, &spotLightBuffer);
	glDeleteBuffers(1, &pointLightBuffer);
	glDeleteBuffers(1, &ambientBuffer);
	glDeleteBuffers(1, &directionalLightBuffer);
	glDeleteBuffers(1, &fogBuffer);
}

void GOLit::UpdateLighting()
{
	if (!Renderer::ShouldDraw())
	{
		return;
	}

	unsigned int textureSlotId = GL_TEXTURE0;

	for (const Material& mat : materials)
	{
		mat.diffuseMap->Bind(textureSlotId++);
		mat.specularMap->Bind(textureSlotId++);
	}

	ambient.ambient = glm::vec4(1.0f) * LightManager::GetAmbientIntensity();

	unsigned int i = 0;
	for (DirectionalLight* dLight : LightManager::GetDirectionalLights(100))
	{
		directionalLight[i].direction = dLight->GetDirection();
		directionalLight[i].color = dLight->GetColor();
		directionalLight[i].lightOn = true;
		i++;
	}

	Camera& cam = CameraManager::GetActiveCamera();

	i = 0;
	for (PointLight* pLight : LightManager::GetPointLights(cam.GetPosition(), 100.0f, 20))
	{
		pointLight[i].position = glm::vec4(pLight->GetPosition(), 1.0f);
		pointLight[i].color = pLight->GetColor();
		pointLight[i].constant = pLight->GetConstant();
		pointLight[i].linear = pLight->GetLinear();
		pointLight[i].quadratic = pLight->GetQuadratic();
		pointLight[i].lightOn = true;
		i++;
	}

	i = 0;
	std::vector<SpotLight*> spotLights = LightManager::GetSpotLights(cam.GetPosition(), 100.0f, 20);
	for (SpotLight* sLight : spotLights)
	{
		spotLight[i].position = glm::vec4(sLight->GetPosition(), 1.0f);
		spotLight[i].color = sLight->GetColor();
		spotLight[i].constant = sLight->GetConstant();
		spotLight[i].linear = sLight->GetLinear();
		spotLight[i].quadratic = sLight->GetQudratic();
		spotLight[i].cutoff = sLight->GetCutoff();
		spotLight[i].outerCutoff = sLight->GetOuterCuttoff();
		spotLight[i].direction = glm::vec4(sLight->GetDirection(), 0.0f);
		spotLight[i].lightOn = true;
		i++;
	}

	glm::vec4 viewPosition = glm::vec4(CameraManager::GetActiveCamera().GetPosition(), 1.0f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, directionalLightBuffer);
	glNamedBufferSubData(directionalLightBuffer, 0, sizeof(directionalLight), &directionalLight);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, ambientBuffer);
	glNamedBufferSubData(ambientBuffer, 0, sizeof(AmbientUBO), &ambient);

	glBindBufferBase(GL_UNIFORM_BUFFER, 4, pointLightBuffer);
	glNamedBufferSubData(pointLightBuffer, 0, sizeof(pointLight), &pointLight);

	glBindBufferBase(GL_UNIFORM_BUFFER, 5, spotLightBuffer);
	glNamedBufferSubData(spotLightBuffer, 0, sizeof(SpotLightUBO) * 20, &spotLight);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 6, viewPositionBuffer);
	glNamedBufferSubData(viewPositionBuffer, 0, sizeof(glm::vec4), &viewPosition);
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 7, materialBuffer);
	glNamedBufferSubData(materialBuffer, 0, sizeof(MaterialUBO), &material);

	glBindBufferBase(GL_UNIFORM_BUFFER, 11, fogBuffer);
	glNamedBufferSubData(fogBuffer, 0, sizeof(FogEffect), &fog);

}

void GOLit::SetShine(float shine)
{
	material.shine = shine;
}

float GOLit::GetShine() const
{
	return material.shine;
}

const std::vector<GOLit::Material>& GOLit::GetMaterials() const
{
	return materials;
}
