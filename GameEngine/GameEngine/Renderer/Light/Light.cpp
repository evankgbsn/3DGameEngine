#include "Light.h"

#include "../../Collision/SphereWithVisualization.h"

const glm::vec4& Light::GetColor() const
{
	return color;
}

Light::Light(const glm::vec4& c) :
	color(c),
	collider()
{
	collider = new SphereWithVisualization(0.1f);
	collider->SetColor({ 1.0f, 1.0f, 0.0f });
}

Light::~Light()
{
	delete collider;
}
