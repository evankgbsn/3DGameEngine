#include "PointLightComponent.h"

#include "../../Renderer/Light/LightManager.h"
#include "../../Renderer/Light/PointLight.h"

PointLightComponent::PointLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position) :
	light(nullptr)
{
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

const std::vector<char> PointLightComponent::Serialize() const
{
	return std::vector<char>();
}

void PointLightComponent::Deserialize(const std::vector<char>& data)
{
}
