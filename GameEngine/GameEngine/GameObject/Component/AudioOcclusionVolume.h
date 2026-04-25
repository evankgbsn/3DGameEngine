#ifndef AUDIOOCCLUSIONVOLUME_H
#define AUDIOOCCLUSIONVOLUME_H

#include "Component.h"

#include <glm/glm.hpp>

#include <string>

class AudioObject;

class AudioOcclusionVolume : public Component
{
public:

	struct Material
	{
		float lowFrequencyAbsorption;
		float midFrequencyAbsorption;
		float highFrequencyAbsorption;

		float scattering;

		float lowFrequencyTransmission;
		float midFrequencyTransmission;
		float highFrequencyTransmission;
	};

	AudioOcclusionVolume();

	AudioOcclusionVolume(const std::string& modelName, const glm::vec3& position, const glm::mat4& rotation, const Material& material);

	~AudioOcclusionVolume();

	void SetPosition(const glm::vec3& position);

	glm::vec3 GetPosition() const;

	void SetRotation(const glm::mat4& rotation);

	glm::mat4 GetRotation() const;

private:

	AudioOcclusionVolume(const AudioOcclusionVolume&) = delete;

	AudioOcclusionVolume& operator=(const AudioOcclusionVolume&) = delete;

	AudioOcclusionVolume(AudioOcclusionVolume&&) = delete;

	AudioOcclusionVolume& operator=(AudioOcclusionVolume&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	AudioObject* obj;
};

#endif // AUDIOOCCLUSIONVOLUME_H