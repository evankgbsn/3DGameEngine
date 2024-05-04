#ifndef SPOTLIGHTCOMPONENT_H
#define SPOTLIGHTCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class SpotLight;

class SpotLightComponent : public Component
{

public:

	SpotLightComponent(const glm::vec3& colorIntensity = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& position = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f));

	~SpotLightComponent();

private:

	SpotLightComponent(const SpotLightComponent&) = delete;

	SpotLightComponent& operator=(const SpotLightComponent&) = delete;

	SpotLightComponent(SpotLightComponent&&) = delete;

	SpotLightComponent& operator=(SpotLightComponent&&) = delete;

	SpotLight* light;
};

#endif // SPOTLIGHTCOMPONENT_H