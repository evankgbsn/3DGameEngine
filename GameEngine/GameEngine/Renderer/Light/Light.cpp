#include "Light.h"

#include "../../Collision/SphereWithVisualization.h"
#include "../Math/Shapes/LineSegment3D.h"
#include "../Camera/Camera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "LightManager.h"

const glm::vec4& Light::GetColor() const
{
	return color;
}

void Light::SetColor(const glm::vec4& newColor)
{
	DirectionalLight* dLight = dynamic_cast<DirectionalLight*>(this);

	if (dLight != nullptr)
	{
		LightManager::SetDirectionalLightUpdated(true);
	}
	else
	{
		PointLight* pLight = dynamic_cast<PointLight*>(this);

		if (pLight != nullptr)
		{
			LightManager::SetPointLightUpdated(true);
		}
		else
		{
			SpotLight* sLight = dynamic_cast<SpotLight*>(this);

			if (sLight != nullptr)
			{
				LightManager::SetSpotLightUpdated(true);
			}
		}
	}

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
