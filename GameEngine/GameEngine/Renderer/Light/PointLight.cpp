#include "PointLight.h"

#include "../../Collision/SphereWithVisualization.h"

#include <glm/gtc/matrix_transform.hpp>

PointLight::PointLight(const glm::vec4& initialColor, const glm::vec3& initialPosition) :
	Light(initialColor),
	position(initialPosition)
{
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, position);

	collider->Transform(transform);
	collider->Update(transform);
}

PointLight::~PointLight()
{
}

const glm::vec3& PointLight::GetPosition() const
{
	return position;
}
