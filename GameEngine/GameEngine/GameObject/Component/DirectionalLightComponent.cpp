#include "DirectionalLightComponent.h"

#include "../../Renderer/Light/LightManager.h"
#include "../../Renderer/Light/DirectionalLight.h"

DirectionalLightComponent::DirectionalLightComponent(const glm::vec3& initialDirection, const glm::vec3& colorIntensity)
{
	light = LightManager::CreateDirectionalLight(glm::vec4(colorIntensity, 1.0f), glm::vec4(initialDirection, 0.0f));
}

DirectionalLightComponent::~DirectionalLightComponent()
{
	LightManager::Delete(light);
}

glm::vec3 DirectionalLightComponent::GetColor() const
{
	return light->GetColor();
}

glm::vec3 DirectionalLightComponent::GetDirection() const
{
	return light->GetDirection();
}

void DirectionalLightComponent::SetDirection(const glm::vec3& newDirection)
{
	light->SetDirection(glm::vec4(newDirection, 1.0f));
}

void DirectionalLightComponent::SetColor(const glm::vec3& newColor)
{
	light->SetColor(glm::vec4(newColor, 1.0f));
}