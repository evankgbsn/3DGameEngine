#include "LightManager.h"

#include "../../Utils/SingletonHelpers.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include <algorithm>

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

		for (unsigned int i = 0; i < instance->directionalLights.size(); ++i)
		{
			if (!IsValid(instance->directionalLights[i]))
			{
				instance->directionalLights[i] = dLight;
				return dLight;
			}
		}

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
		PointLight* pLight = new PointLight(initialColor, position);

		for (unsigned int i = 0; i < instance->pointLights.size(); ++i)
		{
			if (!IsValid(instance->pointLights[i]))
			{
				instance->pointLights[i] = pLight;
				return pLight;
			}
		}

		instance->pointLights.push_back(pLight);
		return pLight;
	}
	else
	{
		Logger::Log("Calling LightMananger::CreatePointLight() before LightManager::Initialize().", Logger::Category::Error);
		return nullptr;
	}
}

SpotLight* const LightManager::CreateSpotLight(const glm::vec4& initialColor, const glm::vec3& initialPosition, const glm::vec3& initialDirection)
{
	if (instance != nullptr)
	{
		SpotLight* spotLight = new SpotLight(initialColor, initialPosition, initialDirection);

		for (unsigned int i = 0; i < instance->spotLights.size(); ++i)
		{
			if (!IsValid(instance->spotLights[i]))
			{
				instance->spotLights[i] = spotLight;
				return spotLight;
			}
		}

		instance->spotLights.push_back(spotLight);
		return spotLight;
	}
	else
	{
		Logger::Log("Calling LightMananger::CreateSpotLight() before LightManager::Initialize().", Logger::Category::Error);
		return nullptr;
	}
}

std::vector<PointLight*> LightManager::GetPointLights(const glm::vec3& fromPosition, float maxDistance, float maxLights)
{
	std::vector<PointLight*> lights;

	if (instance != nullptr)
	{
		for (PointLight* pLight : instance->pointLights)
		{
			if (IsValid(pLight))
			{
				float distance = glm::length(fromPosition - pLight->GetPosition());

				if (distance < maxDistance)
				{
					lights.push_back(pLight);
				}
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::GetPointLights() before LightManager::Initialize()", Logger::Category::Error);
	}

	std::sort(lights.begin(), lights.end(), [fromPosition](PointLight* a, PointLight* b) -> bool
		{
			float distanceA = glm::dot(a->GetPosition() - fromPosition, a->GetPosition() - fromPosition);
			float distanceB = glm::dot(b->GetPosition() - fromPosition, b->GetPosition() - fromPosition);

			return distanceA < distanceB;
		});

	lights = std::vector<PointLight*>(lights.begin(), lights.begin() + (((int)maxLights < lights.size()) ? (int)maxLights : lights.size()));

	return lights;
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
	std::vector<SpotLight*> lights;

	if (instance != nullptr)
	{
		for (SpotLight* pLight : instance->spotLights)
		{
			if (IsValid(pLight))
			{
				float distance = glm::length(fromPosition - pLight->GetPosition());

				if (distance < maxDistance)
				{
					lights.push_back(pLight);
				}
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::GetSpotLights() before LightManager::Initialize()", Logger::Category::Error);
	}

	std::sort(lights.begin(), lights.end(), [fromPosition](SpotLight* a, SpotLight* b) -> bool
		{
			float distanceA = glm::dot(a->GetPosition() - fromPosition, a->GetPosition() - fromPosition);
			float distanceB = glm::dot(b->GetPosition() - fromPosition, b->GetPosition() - fromPosition);

			return distanceA < distanceB;
		});

	lights = std::vector<SpotLight*>(lights.begin(), lights.begin() + (((int)maxLights < lights.size()) ? (int)maxLights : lights.size()));

	return lights;
}

std::vector<SpotLight*> LightManager::GetSpotLightsAtIndices(const std::vector<unsigned int>& indices)
{
	std::vector<SpotLight*> result = std::vector<SpotLight*>();

	if (instance != nullptr)
	{
		for (unsigned int index : indices)
		{
			if (instance->spotLights.size() > index)
			{
				result.push_back(instance->spotLights[index]);
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::GetSpotLightsAtIndices() before LightManager::Initialize()", Logger::Category::Error);
	}

	return result;
}

std::vector<DirectionalLight*> LightManager::GetDirectionalLights(float maxLights)
{
	std::vector<DirectionalLight*> result;

	if (instance != nullptr)
	{
		for(unsigned int i = 0; i < maxLights && i < instance->directionalLights.size(); i++)
		{
			if (IsValid(instance->directionalLights[i]))
				result.push_back(instance->directionalLights[i]);
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

void LightManager::Delete(DirectionalLight* light)
{
	if (instance != nullptr)
	{
		for (unsigned int i = 0; i < instance->directionalLights.size(); ++i)
		{
			if (instance->directionalLights[i] == light)
			{
				delete light;
				instance->directionalLights[i] = (DirectionalLight*)UINT_MAX;
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::Delete(DirectionalLight*) before LightManager::Initialize()", Logger::Category::Warning);
	}
}

void LightManager::Delete(PointLight* light)
{
	if (instance != nullptr)
	{
		for (unsigned int i = 0; i < instance->pointLights.size(); ++i)
		{
			if (instance->pointLights[i] == light)
			{
				delete light;
				instance->pointLights[i] = (PointLight*)UINT_MAX;
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::Delete(PointLight*) before LightManager::Initialize()", Logger::Category::Warning);
	}
}

void LightManager::Delete(SpotLight* light)
{
	if (instance != nullptr)
	{
		for (unsigned int i = 0; i < instance->spotLights.size(); ++i)
		{
			if (instance->spotLights[i] == light)
			{
				delete light;
				instance->spotLights[i] = (SpotLight*)UINT_MAX;
			}
		}
	}
	else
	{
		Logger::Log("Calling LightManager::Delete(SpotLight*) before LightManager::Initialize()", Logger::Category::Warning);
	}
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
		if(IsValid(light))
			delete light;
	}

	for (PointLight* light : pointLights)
	{
		if (IsValid(light))
			delete light;
	}
	
	for (SpotLight* light : spotLights)
	{
		if (IsValid(light))
			delete light;
	}

}

bool LightManager::IsValid(Light* light)
{
	if (instance != nullptr)
	{
		return light != nullptr && light != (Light*)UINT_MAX;
	}
	return false;
}
