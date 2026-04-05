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

	void SetColor(const glm::vec3& newColor);

	void SetPosition(const glm::vec3& newPosition);

	glm::vec3 GetColor() const;

	glm::vec3 GetPosition() const;

	void SetConstant(const float& newConstant);

	void SetLinear(const float& newLinear);

	void SetQuadratic(const float& newQuadratic);

	void SetCutoff(float cutoff);

	void SetOuterCutoff(float outerCutoff);

	const float& GetConstant() const;

	const float& GetLinear() const;

	const float& GetQuadratic() const;

	const float& GetCutoff() const;

	const float& GetOuterCutoff() const;

	bool Hovered() const;

	void SetColliderVisibility(bool visibility);

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