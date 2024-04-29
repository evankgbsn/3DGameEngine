#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

class DirectionalLight : public Light
{
public:

	DirectionalLight(const glm::vec4& color, const glm::vec4& initialDirection);

	~DirectionalLight();

	const glm::vec4& GetDirection() const;

	void SetDirection(const glm::vec4& newDirection);

private:

	DirectionalLight(const DirectionalLight&) = delete;

	DirectionalLight& operator=(const DirectionalLight&) = delete;

	DirectionalLight(DirectionalLight&&) = delete;

	DirectionalLight& operator=(DirectionalLight&&) = delete;

	glm::vec4 direction;

};

#endif // DIRECTIONALLIGHT_H