#include "PointLightComponent.h"

#include "../../Renderer/Light/LightManager.h"
#include "../../Renderer/Light/PointLight.h"

PointLightComponent::PointLightComponent() :
	light(nullptr)
{
	RegisterComponentClassType<PointLightComponent>(this);
	light = LightManager::CreatePointLight(glm::vec4(0.33f, 0.33f, 0.33f, 1.0f), {0.0f, 0.0f, 0.0f});
}

PointLightComponent::PointLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position) :
	light(nullptr)
{
	RegisterComponentClassType<PointLightComponent>(this);
	light = LightManager::CreatePointLight(glm::vec4(colorIntensity, 1.0f), position);
}

PointLightComponent::~PointLightComponent()
{
	LightManager::Delete(light);
}

void PointLightComponent::SetColor(const glm::vec3& newColor)
{
	light->SetColor(glm::vec4(newColor, 1.0f));
}

void PointLightComponent::SetPosition(const glm::vec3& newPosition)
{
	light->SetPosition(newPosition);
}

glm::vec3 PointLightComponent::GetColor() const
{
	return light->GetColor();
}

glm::vec3 PointLightComponent::GetPosition() const
{
	return light->GetPosition();
}

void PointLightComponent::Serialize()
{
	savedVec3s["ColorIntensity"] = GetColor();
	savedVec3s["Position"] = GetPosition();
}

void PointLightComponent::Deserialize()
{
	light = LightManager::CreatePointLight(glm::vec4(savedVec3s["ColorIntensity"], 1.0f), savedVec3s["Position"]);
}

void PointLightComponent::Update()
{
}

void PointLightComponent::SetConstant(const float& newConstant)
{
	if (light != nullptr)
	{
		light->SetConstant(newConstant);
	}
}

void PointLightComponent::SetLinear(const float& newLinear)
{
	if (light != nullptr)
	{
		light->SetLinear(newLinear);
	}
}

void PointLightComponent::SetQuadratic(const float& newQuadratic)
{
	if (light != nullptr)
	{
		light->SetQuadratic(newQuadratic);
	}
}

const float& PointLightComponent::GetConstant() const
{
	if (light != nullptr)
	{
		return light->GetConstant();
	}

	return 0.0f;
}

const float& PointLightComponent::GetLinear() const
{
	if (light != nullptr)
	{
		return light->GetLinear();
	}

	return 0.0f;
}

const float& PointLightComponent::GetQuadratic() const
{
	if (light != nullptr)
	{
		return light->GetQuadratic();
	}

	return 0.0f;
}

bool PointLightComponent::Hovered() const
{
	if (light != nullptr)
	{
		return light->Hovered();
	}

	return false;
}

void PointLightComponent::SetColliderVisibility(bool visibility)
{
	if (light != nullptr)
	{
		return light->SetColliderVisibility(visibility);
	}
}