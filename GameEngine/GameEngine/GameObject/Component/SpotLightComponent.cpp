#include "SpotLightComponent.h"

#include "../../Renderer/Light/LightManager.h"
#include "../../Renderer/Light/SpotLight.h"

SpotLightComponent::SpotLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position, const glm::vec3& direction)
{
	light = LightManager::CreateSpotLight(glm::vec4(colorIntensity, 1.0f), position, direction);
}

SpotLightComponent::~SpotLightComponent()
{
	LightManager::Delete(light);
}
