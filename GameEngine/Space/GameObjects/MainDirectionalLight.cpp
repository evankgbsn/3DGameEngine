#include "MainDirectionalLight.h"

#include "GameEngine/GameObject/Component/DirectionalLightComponent.h"
#include "GameEngine/Renderer/Light/LightManager.h"

MainDirectionalLight::MainDirectionalLight() :
	GameObject("MainDirectionalLight")
{
}

MainDirectionalLight::~MainDirectionalLight()
{
}

void MainDirectionalLight::Initialize()
{
	light = new DirectionalLightComponent({ 0.33f, 0.33f, 0.33f }, { 0.0f, 1.0f, 1.0f });
	LightManager::SetAmbientIntensity(0.8f);
	AddComponent(light, "DirectionalLight");
}

void MainDirectionalLight::Terminate()
{
	RemoveComponent("DirectionalLight");
	delete light;
}

void MainDirectionalLight::GameUpdate()
{
}

void MainDirectionalLight::EditorUpdate()
{
}

void MainDirectionalLight::Load()
{
}

void MainDirectionalLight::Unload()
{
}

bool MainDirectionalLight::Hovered() const
{
	return false;
}

glm::vec3 MainDirectionalLight::GetPosition() const
{
	return glm::vec3();
}

void MainDirectionalLight::SetPosition(const glm::vec3& newPos)
{
}

glm::mat4 MainDirectionalLight::GetRotation() const
{
	return glm::mat4();
}

void MainDirectionalLight::SetRotation(const glm::mat4& newRot)
{
}

glm::mat4 MainDirectionalLight::GetTransform() const
{
	return glm::mat4();
}