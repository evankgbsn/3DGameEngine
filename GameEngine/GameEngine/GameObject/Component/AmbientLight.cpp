#include "AmbientLight.h"

#include "../../Renderer/Light/LightManager.h"

AmbientLight::AmbientLight()
{
	RegisterComponentClassType<AmbientLight>(this);
}

AmbientLight::AmbientLight(float intensity)
{
	RegisterComponentClassType<AmbientLight>(this);
	LightManager::SetAmbientIntensity(intensity);
}

AmbientLight::~AmbientLight()
{
	LightManager::SetAmbientIntensity(0.0f);
}

float AmbientLight::GetIntensity() const
{
	return LightManager::GetAmbientIntensity();
}

void AmbientLight::SetAmbientIntensity(float intensity)
{
	LightManager::SetAmbientIntensity(intensity);
}

void AmbientLight::Serialize()
{
	savedFloats["intensity"] = LightManager::GetAmbientIntensity();
}

void AmbientLight::Deserialize()
{
	LightManager::SetAmbientIntensity(savedFloats["intensity"]);
}

void AmbientLight::Update()
{
}
