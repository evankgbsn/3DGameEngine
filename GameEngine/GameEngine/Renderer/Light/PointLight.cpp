#include "PointLight.h"

#include "../../Collision/SphereWithVisualization.h"
#include "../Shader/ShaderManager.h"
#include "LightManager.h"


#include <glm/gtc/matrix_transform.hpp>

#include <gl/glew.h>

PointLight::PointLight(const glm::vec4& initialColor, const glm::vec3& initialPosition) :
	Light(initialColor),
	position(initialPosition),
	constant(1.0f),
	linear(0.09f),
	quadratic(0.032f),
	farPlane(25.0f)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, position);

	collider->Transform(transform);
	collider->Update(transform);

	LightManager::SetPointLightUpdated(true);
}

PointLight::~PointLight()
{
}

const glm::vec3& PointLight::GetPosition() const
{
	return position;
}

const float& PointLight::GetConstant() const
{
	return constant;
}

const float& PointLight::GetLinear() const
{
	return linear;
}

const float& PointLight::GetQuadratic() const
{
	return quadratic;
}

void PointLight::SetPosition(const glm::vec3& newPosition)
{
	position = newPosition;

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, position);

	collider->Transform(transform);
	collider->Update(transform);

	LightManager::SetPointLightUpdated(true);
}

void PointLight::SetConstant(const float& newConstant)
{
	constant = newConstant;

	LightManager::SetPointLightUpdated(true);
}

void PointLight::SetLinear(const float& newLinear)
{
	linear = newLinear;

	LightManager::SetPointLightUpdated(true);
}

void PointLight::SetQuadratic(const float& newQuadratic)
{
	quadratic = newQuadratic;

	LightManager::SetPointLightUpdated(true);
}