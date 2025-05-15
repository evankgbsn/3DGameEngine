#ifndef SPOTLIGHTCOMPONENT_H
#define SPOTLIGHTCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class SpotLight;

class SpotLightComponent : public Component
{

public:

	SpotLightComponent();

	SpotLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position, const glm::vec3& direction);

	~SpotLightComponent();

private:

	SpotLightComponent(const SpotLightComponent&) = delete;

	SpotLightComponent& operator=(const SpotLightComponent&) = delete;

	SpotLightComponent(SpotLightComponent&&) = delete;

	SpotLightComponent& operator=(SpotLightComponent&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	SpotLight* light;
};

#endif // SPOTLIGHTCOMPONENT_H