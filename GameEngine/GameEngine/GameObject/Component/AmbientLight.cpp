#include "AmbientLight.h"

#include "../../Renderer/Light/LightManager.h"

AmbientLight::AmbientLight(float intensity)
{
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
