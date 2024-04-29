#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

#include <vector>

class PointLight : public Light
{

public:

	PointLight(const glm::vec4& initialColor, const glm::vec3& initialPosition);

	~PointLight();

	const glm::vec3& GetPosition() const;

	const float& GetConstant() const;

	const float& GetLinear() const;

	const float& GetQuadratic() const;

	void SetPosition(const glm::vec3& newPosition);

	void SetConstant(const float& newConstant);

	void SetLinear(const float& newLinear);

	void SetQuadratic(const float& newQuadratic);

protected:

private:

	PointLight();

	PointLight(const PointLight&) = delete;

	PointLight& operator=(const PointLight&) = delete;

	PointLight(PointLight&&) = delete;

	PointLight& operator=(PointLight&&) = delete;

	glm::vec3 position;

	float constant;

	float linear;

	float quadratic;

	unsigned int depthCubeMap;

	float farPlane;

	std::vector<glm::mat4> shadowTransforms;

};

#endif // POINTLIGHT_H