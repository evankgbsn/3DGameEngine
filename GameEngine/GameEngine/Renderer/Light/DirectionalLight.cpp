#include "DirectionalLight.h"

#include "LightManager.h"

DirectionalLight::DirectionalLight(const glm::vec4& color, const glm::vec4& initialDirection) :
	Light(color),
	direction(initialDirection)
{
	LightManager::SetDirectionalLightUpdated(true);
}

DirectionalLight::~DirectionalLight()
{
}

const glm::vec4& DirectionalLight::GetDirection() const
{
	return direction;
}

void DirectionalLight::SetDirection(const glm::vec4& newDirection)
{
	direction = newDirection;

	LightManager::SetDirectionalLightUpdated(true);
}
