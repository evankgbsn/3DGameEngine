#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class SphereWithVisualization;

class Light
{

public:

	const glm::vec4& GetColor() const;

	void SetColor(const glm::vec4& newColor);

	SphereWithVisualization* GetCollider() const;

	bool Hovered() const;

	void SetColliderVisibility(bool visibility);

protected:

	Light(const glm::vec4& color);

	virtual ~Light();

	SphereWithVisualization* collider;

	glm::vec4 color;

private:

	Light(const Light&) = delete;

	Light& operator=(const Light&) = delete;

	Light(Light&&) = delete;

	Light& operator=(Light&&) = delete;


};

#endif // LIGHT_H