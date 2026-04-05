#include "SpotLightComponent.h"

#include "../../Renderer/Light/LightManager.h"
#include "../../Renderer/Light/SpotLight.h"

SpotLightComponent::SpotLightComponent() :
	light(nullptr)
{
	RegisterComponentClassType<SpotLightComponent>(this);
}

SpotLightComponent::SpotLightComponent(const glm::vec3& colorIntensity, const glm::vec3& position, const glm::vec3& direction) :
	light(nullptr)
{
	RegisterComponentClassType<SpotLightComponent>(this);
	light = LightManager::CreateSpotLight(glm::vec4(colorIntensity, 1.0f), position, direction);
}

SpotLightComponent::~SpotLightComponent()
{
	LightManager::Delete(light);
}

void SpotLightComponent::SetColor(const glm::vec3& newColor)
{
	if (light != nullptr)
	{
		light->SetColor(glm::vec4(newColor, 1.0f));
	}
}

void SpotLightComponent::SetPosition(const glm::vec3& newPosition)
{
	if (light != nullptr)
	{
		light->SetPosition(newPosition);
	}
}

glm::vec3 SpotLightComponent::GetColor() const
{
	if (light != nullptr)
	{
		return light->GetColor();
	}

	return glm::vec3(0.0f);
}

glm::vec3 SpotLightComponent::GetPosition() const
{
	if (light != nullptr)
	{
		return light->GetPosition();
	}

	return glm::vec3(0.0f);
}

void SpotLightComponent::Serialize()
{
	savedVec3s["Position"] = light->GetPosition();
	savedVec3s["Direction"] = light->GetDirection();
	savedVec3s["ColorIntensity"] = light->GetColor();
	savedFloats["Linear"] = GetLinear();
	savedFloats["Constant"] = GetConstant();
	savedFloats["Quadratic"] = GetQuadratic();
	savedFloats["Cutoff"] = GetCutoff();
	savedFloats["OuterCutoff"] = GetOuterCutoff();
}

void SpotLightComponent::Deserialize()
{
	if (light != nullptr)
	{
		LightManager::Delete(light);
	}

	light = LightManager::CreateSpotLight(glm::vec4(savedVec3s["ColorIntensity"], 1.0f), savedVec3s["Position"], savedVec3s["Direction"]);

	light->SetLinear(savedFloats["Linear"]);
	light->SetConstant(savedFloats["Constant"]);
	light->SetQuadratic(savedFloats["Quadratic"]);
	light->SetCutoff(savedFloats["Cutoff"]);
	light->SetOuterCutoff(savedFloats["OuterCutoff"]);
}

void SpotLightComponent::Update()
{
}

void SpotLightComponent::SetConstant(const float& newConstant)
{
	if (light != nullptr)
	{
		light->SetConstant(newConstant);
	}
}

void SpotLightComponent::SetLinear(const float& newLinear)
{
	if (light != nullptr)
	{
		light->SetLinear(newLinear);
	}
}

void SpotLightComponent::SetQuadratic(const float& newQuadratic)
{
	if (light != nullptr)
	{
		light->SetQuadratic(newQuadratic);
	}
}

void SpotLightComponent::SetCutoff(float cutoff)
{
	if (light != nullptr)
	{
		light->SetCutoff(cutoff);
	}
}

void SpotLightComponent::SetOuterCutoff(float outerCutoff)
{
	if (light != nullptr)
	{
		light->SetOuterCutoff(outerCutoff);
	}
}

const float& SpotLightComponent::GetConstant() const
{
	if (light != nullptr)
	{
		return light->GetConstant();
	}

	return 0.0f;
}

const float& SpotLightComponent::GetLinear() const
{
	if (light != nullptr)
	{
		return light->GetLinear();
	}

	return 0.0f;
}

const float& SpotLightComponent::GetQuadratic() const
{
	if (light != nullptr)
	{
		return light->GetQuadratic();
	}

	return 0.0f;
}

const float& SpotLightComponent::GetCutoff() const
{
	if (light != nullptr)
	{
		return light->GetCutoff();
	}

	return 0.0f;
}

const float& SpotLightComponent::GetOuterCutoff() const
{
	if (light != nullptr)
	{
		return light->GetOuterCuttoff();
	}

	return 0.0f;
}

bool SpotLightComponent::Hovered() const
{
	if (light != nullptr)
	{
		return light->Hovered();
	}

	return false;
}

void SpotLightComponent::SetColliderVisibility(bool visibility)
{
	if (light != nullptr)
	{
		return light->SetColliderVisibility(visibility);
	}
}
