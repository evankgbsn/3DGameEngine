#ifndef DIRECTIONALLIGHTCOMPONENT_H
#define DIRECTIONALLIGHTCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class DirectionalLight;
class SphereWithVisualization;

class DirectionalLightComponent : public Component
{

public:

	DirectionalLightComponent() = default;

	DirectionalLightComponent(const glm::vec3& initialDirection, const glm::vec3& colorIntensity);

	~DirectionalLightComponent();

	glm::vec3 GetDirection() const;

	glm::vec3 GetColor() const;

	SphereWithVisualization* const GetCollider() const;

	void SetDirection(const glm::vec3& newDirection);

	void SetColor(const glm::vec3& newColor);

	void Update() override;

private:

	DirectionalLightComponent(const DirectionalLightComponent&) = delete;

	DirectionalLightComponent& operator=(const DirectionalLightComponent&) = delete;

	DirectionalLightComponent(DirectionalLightComponent&&) = delete;

	DirectionalLightComponent& operator=(DirectionalLightComponent&&) = delete;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	DirectionalLight* light;

};

#endif // DIRECTIONALLIGHTCOMPONENT_H