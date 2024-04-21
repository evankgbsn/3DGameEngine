#include "LightManager.h"

#include "../../Utils/SingletonHelpers.h"
#include "DirectionalLight.h"
#include "PointLight.h"

LightManager* LightManager::instance = nullptr;

void LightManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "LightManager");
}

void LightManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "LightManager");
}

void LightManager::SetAmbientIntensity(float intensity)
{
	if (instance != nullptr)
	{
		instance->ambientIntensity = intensity;
	}
	else
	{
		Logger::Log("Calling LightManager::SetAmbientIntensity() before LightManager::Initialize()", Logger::Category::Error);
	}
}

float LightManager::GetAmbientIntensity()
{
	if (instance != nullptr)
	{
		return instance->ambientIntensity;
	}
	else
	{
		Logger::Log("Calling LightManager::GetAmbientIntensity() before LightManager::Initialize()", Logger::Category::Error);
		return 0.0f;
	}

}

DirectionalLight* const LightManager::CreateDirectionalLight(const glm::vec4& initialColor, const glm::vec4& initialDirection)
{
	if (instance != nullptr)
	{
		DirectionalLight* dLight = new DirectionalLight(initialColor, initialDirection);
		instance->directionalLights.push_back(dLight);
		return dLight;
	}
	else
	{
		Logger::Log("Calling LightMananger::CreateDirectionalLight() before LightManager::Initialize().", Logger::Category::Error);
		return nullptr;
	}
}

PointLight* const LightManager::CreatePointLight(const glm::vec4& initialColor, const glm::vec3& position)
{
	if (instance != nullptr)
	{
		PointLight* dLight = new PointLight(initialColor, position);
		instance->pointLights.push_back(dLight);
		return dLight;
	}
	else
	{
		Logger::Log("Calling LightMananger::CreatePointLight() before LightManager::Initialize().", Logger::Category::Error);
		return nullptr;
	}
}

std::vector<PointLight*> LightManager::GetPointLights(const glm::vec3& fromPosition, float maxDistance, float maxLights)
{
	return std::vector<PointLight*>();
}

std::vector<PointLight*> LightManager::GetPointLightsAtIndices(const std::vector<unsigned int>& indices)
{
	std::vector<PointLight*> result = std::vector<PointLight*>();

	if (instance != nullptr)
	{
		for (unsigned int index : indices)
		{
			if (instance->pointLights.size() > index)
			{
				result.push_back(instance->pointLights[index]);
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::GetPointLightsAtIndices() before LightManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

std::vector<SpotLight*> LightManager::GetSpotLights(const glm::vec3& fromPosition, float maxDistance, float maxLights)
{
	return std::vector<SpotLight*>();
}

std::vector<DirectionalLight*> LightManager::GetDirectionalLights(float maxLights)
{
	std::vector<DirectionalLight*> result;

	if (instance != nullptr)
	{
		if (instance->directionalLights.size() < maxLights)
		{
			result = instance->directionalLights;
		}
		else
		{
			for (unsigned int i = 0; i < maxLights; i++)
			{
				result.push_back(instance->directionalLights[i]);
			}
		}
	}

	return result;
}

std::vector<DirectionalLight*> LightManager::GetDirectionalLightsAtIndices(const std::vector<unsigned int>& indices)
{
	std::vector<DirectionalLight*> result = std::vector<DirectionalLight*>();

	if (instance != nullptr)
	{
		for (unsigned int index : indices)
		{
			if (instance->directionalLights.size() > index)
			{
				result.push_back(instance->directionalLights[index]);
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::GetDirectionalLightsAtIndices() before LightManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

LightManager::LightManager() :
	directionalLights(std::vector<DirectionalLight*>()),
	pointLights(std::vector<PointLight*>()),
	spotLights(std::vector<SpotLight*>())
{
}

LightManager::~LightManager()
{
	for (DirectionalLight* light : directionalLights)
	{
		delete light;
	}
}
