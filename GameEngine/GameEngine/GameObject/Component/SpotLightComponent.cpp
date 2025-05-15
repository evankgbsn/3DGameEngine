#include "SpotLightComponent.h"

#include "../../Renderer/Light/LightManager.h"
#include "../../Renderer/Light/SpotLight.h"

SpotLightComponent::SpotLightComponent()
{
	RegisterComponentClassType<SpotLightComponent>(this);
}

SpotLightComponent::SpotLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position, const glm::vec3& direction)
{
	RegisterComponentClassType<SpotLightComponent>(this);
	light = LightManager::CreateSpotLight(glm::vec4(colorIntensity, 1.0f), position, direction);
}

SpotLightComponent::~SpotLightComponent()
{
	LightManager::Delete(light);
}

void SpotLightComponent::Serialize()
{
	savedVec3s["Position"] = light->GetPosition();
	savedVec3s["Direction"] = light->GetDirection();
	savedVec3s["ColorIntensity"] = light->GetColor();
}

void SpotLightComponent::Deserialize()
{
	light = LightManager::CreateSpotLight(glm::vec4(savedVec3s["ColorIntensity"], 1.0f), savedVec3s["Position"], savedVec3s["Direction"]);
}

void SpotLightComponent::Update()
{
}
