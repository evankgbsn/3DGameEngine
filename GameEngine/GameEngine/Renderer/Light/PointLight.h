#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light
{

public:

	PointLight(const glm::vec4& initialColor, const glm::vec3& initialPosition);

	~PointLight();

	const glm::vec3& GetPosition() const;

protected:

private:

	PointLight();

	PointLight(const PointLight&) = delete;

	PointLight& operator=(const PointLight&) = delete;

	PointLight(PointLight&&) = delete;

	PointLight& operator=(PointLight&&) = delete;

	glm::vec3 position;

};

#endif // POINTLIGHT_H