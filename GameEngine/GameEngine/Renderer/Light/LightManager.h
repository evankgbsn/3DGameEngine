#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <glm/glm.hpp>

#include <vector>

class DirectionalLight;
class PointLight;
class SpotLight;

class LightManager
{

public:

	static void Initialize();

	static void Terminate();

	static void SetAmbientIntensity(float intensity);

	static float GetAmbientIntensity();

	static DirectionalLight* const CreateDirectionalLight(const glm::vec4& initialColor, const glm::vec4& initialDirection);

	static PointLight* const CreatePointLight(const glm::vec4& initialColor, const glm::vec3& position);

	static std::vector<PointLight*> GetPointLights(const glm::vec3& fromPosition, float maxDistance, float maxLights);

	static std::vector<PointLight*> GetPointLightsAtIndices(const std::vector<unsigned int>& indices);
	
	static std::vector<SpotLight*> GetSpotLights(const glm::vec3& fromPosition, float maxDistance, float maxLights);

	static std::vector<DirectionalLight*> GetDirectionalLights(float maxLights);

	static std::vector<DirectionalLight*> GetDirectionalLightsAtIndices(const std::vector<unsigned int>& indices);

private:

	friend class SingletonHelpers;

	LightManager();

	~LightManager();

	LightManager(const LightManager&) = delete;

	LightManager& operator=(const LightManager&) = delete;

	LightManager(LightManager&&) = delete;

	LightManager& operator=(LightManager&&) = delete;

	static LightManager* instance;

	std::vector<DirectionalLight*> directionalLights;

	std::vector<SpotLight*> spotLights;

	std::vector<PointLight*> pointLights;

	float ambientIntensity;

};

#endif // LIGHTMANAGER_H