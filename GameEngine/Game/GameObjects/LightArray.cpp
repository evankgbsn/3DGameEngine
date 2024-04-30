#include "LightArray.h"

#include "GameEngine/Renderer/Light/SpotLight.h"
#include "GameEngine/Renderer/Light/LightManager.h"

LightArray::LightArray()
{
}

LightArray::~LightArray()
{
}

void LightArray::Initialize()
{
	float count = 10;
	float dist = 5.0f;
	spotLights.reserve(count * count);
	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < count; ++j)
		{
			spotLights.push_back(LightManager::CreateSpotLight({ 5.0f, 5.0f, 5.0f, 0.0f }, { dist * i, 5.0f, dist * j }, { 0.0f, -1.0f, 0.3f }));
		}
	}

	pointLight = LightManager::CreatePointLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { -1.0f, 4.0f, 4.5f });
}

void LightArray::Terminate()
{
	for (SpotLight* spotLight : spotLights)
	{
		LightManager::Delete(spotLight);
	}

	LightManager::Delete(pointLight);
}

void LightArray::Update()
{
}

void LightArray::Load()
{
}

void LightArray::Unload()
{
}
