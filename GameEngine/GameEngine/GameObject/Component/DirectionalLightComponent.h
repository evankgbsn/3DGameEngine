#ifndef DIRECTIONALLIGHTCOMPONENT_H
#define DIRECTIONALLIGHTCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class DirectionalLight;

class DirectionalLightComponent : public Component
{

public:

	DirectionalLightComponent(const glm::vec3& initialDirection = glm::vec3(0.5f, 0.5f, 0.5f), const glm::vec3& colorIntensity = glm::vec3(1.0f, 1.0f, 1.0f));

	~DirectionalLightComponent();

	glm::vec3 GetDirection() const;

	glm::vec3 GetColor() const;

	void SetDirection(const glm::vec3& newDirection);

	void SetColor(const glm::vec3& newColor);

private:

	DirectionalLightComponent(const DirectionalLightComponent&) = delete;

	DirectionalLightComponent& operator=(const DirectionalLightComponent&) = delete;

	DirectionalLightComponent(DirectionalLightComponent&&) = delete;

	DirectionalLightComponent& operator=(DirectionalLightComponent&&) = delete;

	DirectionalLight* light;

};

#endif // DIRECTIONALLIGHTCOMPONENT_H