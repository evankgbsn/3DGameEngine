#include "SpotLight.h"

#include "../../Collision/SphereWithVisualization.h"

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
	return cutoff;
}

const float& SpotLight::GetOuterCuttoff() const
{
	return outerCutoff;
}

const float& SpotLight::GetConstant() const
{
	return constant;
}

const float& SpotLight::GetLinear() const
{
	return linear;
}

const float& SpotLight::GetQudratic() const
{
	return quadratic;
}
