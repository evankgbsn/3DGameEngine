#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "Light.h"

class SpotLight : public Light
{

public:

	SpotLight(const glm::vec4& initialColor, const glm::vec3& initialPosition, const glm::vec3& initialDirection);

	~SpotLight();

	const glm::vec3& GetPosition() const;

	const glm::vec3& GetDirection() const;

	const float& GetCutoff() const;

	void SetCutoff(float newCutoff);

	const float& GetOuterCuttoff() const;

	void SetOuterCutoff(float newOuterCutoff);

	const float& GetConstant() const;

	void SetConstant(float newConstant);

	const float& GetLinear() const;

	void SetLinear(float newLinear);

	const float& GetQuadratic() const;

	void SetQuadratic(float newQuadratic);

	void SetPosition(const glm::vec3& pos);

protected:

private:

	SpotLight(const SpotLight&) = delete;

	SpotLight& operator=(const SpotLight&) = delete;

	SpotLight(SpotLight&&) = delete;

	SpotLight& operator=(SpotLight&&) = delete;

	glm::vec3 position;

	glm::vec3 direction;

	float cutoff;

	float outerCutoff;

	float constant;

	float linear;

	float quadratic;


};

#endif // SPOTLIGHT_H