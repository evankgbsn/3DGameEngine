#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class SphereWithVisualization;

class Light
{

public:

	const glm::vec4& GetColor() const;

protected:

	Light(const glm::vec4& color);

	~Light();

	SphereWithVisualization* collider;

	glm::vec4 color;

private:

	Light(const Light&) = delete;

	Light& operator=(const Light&) = delete;

	Light(Light&&) = delete;

	Light& operator=(Light&&) = delete;


};

#endif // LIGHT_H