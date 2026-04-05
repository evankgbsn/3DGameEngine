#include "SpotLight.h"

#include "../../Collision/SphereWithVisualization.h"
#include "LightManager.h"

#include <glm/gtc/matrix_transform.hpp>

SpotLight::SpotLight(const glm::vec4& initialColor, const glm::vec3& initialPosition, const glm::vec3& initialDirection) :
	Light(initialColor),
	position(initialPosition),
	direction(initialDirection),
	cutoff(glm::cos(glm::radians(12.5f))),
	outerCutoff(glm::cos(glm::radians(17.5f))),
	constant(1.0f),
	linear(0.09f),
	quadratic(0.032f)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, position);

	collider->Transform(transform);
	collider->Update(transform);

	LightManager::SetSpotLightUpdated(true);
}

SpotLight::~SpotLight()
{
}

const glm::vec3& SpotLight::GetPosition() const
{
	return position;
}

const glm::vec3& SpotLight::GetDirection() const
{
	return direction;
}

const float& SpotLight::GetCutoff() const
{
	return glm::degrees(glm::acos(cutoff));
}

void SpotLight::SetCutoff(float newCutoff)
{
	cutoff = glm::cos(glm::radians(newCutoff));

	LightManager::SetSpotLightUpdated(true);
}

const float& SpotLight::GetOuterCuttoff() const
{
	return glm::degrees(glm::acos(outerCutoff));
}

void SpotLight::SetOuterCutoff(float newOuterCutoff)
{
	outerCutoff = glm::cos(glm::radians(newOuterCutoff));

	LightManager::SetSpotLightUpdated(true);
}

const float& SpotLight::GetConstant() const
{
	return constant;
}

void SpotLight::SetConstant(float newConstant)
{

	constant = newConstant;

	LightManager::SetSpotLightUpdated(true);
}

const float& SpotLight::GetLinear() const
{
	return linear;
}

void SpotLight::SetLinear(float newLinear)
{
	linear = newLinear;

	LightManager::SetSpotLightUpdated(true);
}

const float& SpotLight::GetQuadratic() const
{
	return quadratic;
}

void SpotLight::SetQuadratic(float newQuadratic)
{
	quadratic = newQuadratic;

	LightManager::SetSpotLightUpdated(true);
}

void SpotLight::SetPosition(const glm::vec3& pos)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, pos);

	collider->Transform(transform);
	collider->Update(transform);

	position = pos;

	LightManager::SetSpotLightUpdated(true);
}
