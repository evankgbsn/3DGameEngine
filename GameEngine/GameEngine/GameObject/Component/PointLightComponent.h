#ifndef POINTLIGHTCOMPONENT_H
#define POINTLIGHTCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class PointLight;

class PointLightComponent : public Component
{

public:

	PointLightComponent();

	PointLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position);

	~PointLightComponent();

	void SetColor(const glm::vec3& newColor);

	void SetPosition(const glm::vec3& newPosition);

	glm::vec3 GetColor() const;

	glm::vec3 GetPosition() const;

private:

	PointLightComponent(const PointLightComponent&) = delete;

	PointLightComponent& operator=(const PointLightComponent&) = delete;

	PointLightComponent(PointLightComponent&&) = delete;

	PointLightComponent& operator=(PointLightComponent&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	PointLight* light;

};


#endif // POINTLIGHTCOMPONENT_H