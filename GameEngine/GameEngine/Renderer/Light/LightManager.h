#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <glm/glm.hpp>

#include <vector>

class DirectionalLight;
class PointLight;
class SpotLight;
class Light;

class LightManager
{

public:

	static void Initialize();

	static void Terminate();

	static void SetAmbientIntensity(float intensity);

	static float GetAmbientIntensity();

	static DirectionalLight* const CreateDirectionalLight(const glm::vec4& initialColor, const glm::vec4& initialDirection);

	static PointLight* const CreatePointLight(const glm::vec4& initialColor, const glm::vec3& position);

	static SpotLight* const CreateSpotLight(const glm::vec4& initialColor, const glm::vec3& initialPosition, const glm::vec3& initialDirection);

	static std::vector<PointLight*> GetPointLights(const glm::vec3& fromPosition, float maxDistance, float maxLights);

	static std::vector<PointLight*> GetPointLightsAtIndices(const std::vector<unsigned int>& indices);
	
	static std::vector<SpotLight*> GetSpotLights(const glm::vec3& fromPosition, float maxDistance, float maxLights);

	static std::vector<SpotLight*> GetSpotLightsAtIndices(const std::vector<unsigned int>& indices);

	static std::vector<DirectionalLight*> GetDirectionalLights(float maxLights);

	static std::vector<DirectionalLight*> GetDirectionalLightsAtIndices(const std::vector<unsigned int>& indices);

	static void Delete(DirectionalLight* light);

	static void Delete(PointLight* light);

	static void Delete(SpotLight* light);

private:

	friend class SingletonHelpers;

	LightManager();

	~LightManager();

	LightManager(const LightManager&) = delete;

	LightManager& operator=(const LightManager&) = delete;

	LightManager(LightManager&&) = delete;

	LightManager& operator=(LightManager&&) = delete;

	static bool IsValid(Light* light);

	static LightManager* instance;

	std::vector<DirectionalLight*> directionalLights;

	std::vector<SpotLight*> spotLights;

	std::vector<PointLight*> pointLights;

	float ambientIntensity;

};

#endif // LIGHTMANAGER_H