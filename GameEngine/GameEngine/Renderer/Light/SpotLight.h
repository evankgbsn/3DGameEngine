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

	const float& GetOuterCuttoff() const;

	const float& GetConstant() const;

	const float& GetLinear() const;

	const float& GetQudratic() const;

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