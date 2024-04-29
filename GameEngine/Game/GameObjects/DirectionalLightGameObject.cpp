#include "DirectionalLightGameObject.h"

#include "GameEngine/Renderer/Light/LightManager.h"
#include "GameEngine/Renderer/Light/DirectionalLight.h"

#include <glm/gtc/matrix_transform.hpp>

DirectionalLightGameObject::DirectionalLightGameObject() :
	light(nullptr)
{
}

DirectionalLightGameObject::~DirectionalLightGameObject()
{
}

void DirectionalLightGameObject::Initialize()
{
	light = LightManager::CreateDirectionalLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { -0.5f, -0.5f, 0.5f, 0.0f });
	LightManager::SetAmbientIntensity(0.02f);
}

void DirectionalLightGameObject::Terminate()
{
}

void DirectionalLightGameObject::Update()
{
	glm::mat4 rot(1.0f);
	static float rotationAngle = 0.0001f;

	rot = glm::rotate(rot, rotationAngle, glm::vec3(1.0f, 0.0f, 0.0f));

	light->SetDirection(rot * light->GetDirection());
}

void DirectionalLightGameObject::Load()
{
}

void DirectionalLightGameObject::Unload()
{
}
