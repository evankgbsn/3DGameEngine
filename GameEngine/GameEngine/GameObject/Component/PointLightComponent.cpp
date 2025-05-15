#include "PointLightComponent.h"

#include "../../Renderer/Light/LightManager.h"
#include "../../Renderer/Light/PointLight.h"

PointLightComponent::PointLightComponent()
{
	RegisterComponentClassType<PointLightComponent>(this);
}

PointLightComponent::PointLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position) :
	light(nullptr)
{
	RegisterComponentClassType<PointLightComponent>(this);
	light = LightManager::CreatePointLight(glm::vec4(colorIntensity, 1.0f), position);
}

PointLightComponent::~PointLightComponent()
{
	LightManager::Delete(light);
}

void PointLightComponent::SetColor(const glm::vec3& newColor)
{
	light->SetColor(glm::vec4(newColor, 1.0f));
}

void PointLightComponent::SetPosition(const glm::vec3& newPosition)
{
	light->SetPosition(newPosition);
}

glm::vec3 PointLightComponent::GetColor() const
{
	return light->GetColor();
}

glm::vec3 PointLightComponent::GetPosition() const
{
	return light->GetPosition();
}

void PointLightComponent::Serialize()
{
	savedVec3s["ColorIntensity"] = GetColor();
	savedVec3s["Position"] = GetPosition();
}

void PointLightComponent::Deserialize()
{
	light = LightManager::CreatePointLight(glm::vec4(savedVec3s["ColorIntensity"], 1.0f), savedVec3s["Position"]);
}

void PointLightComponent::Update()
{
}
