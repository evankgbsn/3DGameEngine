#include "Light.h"

#include "../../Collision/SphereWithVisualization.h"
#include "../Math/Shapes/LineSegment3D.h"
#include "../Camera/Camera.h"

const glm::vec4& Light::GetColor() const
{
	return color;
}

void Light::SetColor(const glm::vec4& newColor)
{
	color = newColor;
}

SphereWithVisualization* Light::GetCollider() const
{
	return collider;
}

bool Light::Hovered() const
{
	if (collider != nullptr)
	{
		LineSegment3D lineFromCam = Camera::CastLineFromCursorWithActiveCamera();
		return collider->LineSegmentIntersect(lineFromCam);
	}

	return false;
}

void Light::SetColliderVisibility(bool visibility)
{
	if (collider->IsVisible() && !visibility)
	{
		collider->ToggleVisibility();
	} 
	else if (!collider->IsVisible() && visibility)
	{
		collider->ToggleVisibility();
	}
}

Light::Light(const glm::vec4& c) :
	color(c),
	collider(nullptr)
{
	collider = new SphereWithVisualization(0.3f);
	collider->SetColor({ 1.0f, 1.0f, 0.0f });
}

Light::~Light()
{
	delete collider;
}
