#ifndef AUDIOLISTENER_H
#define AUDIOLISTENER_H

#include "Component.h"

#include <glm/glm.hpp>

class Listener;

class AudioListener : public Component
{
public:

	AudioListener(const glm::vec3& position, const glm::mat4& rotation);

	AudioListener();

	~AudioListener();

	void SetPosition(const glm::vec3& pos);

	glm::vec3 GetPosition() const;

	void SetRotation(const glm::mat4& rot);

	glm::mat4 GetRotation() const;

	glm::vec3 GetForward() const;

	glm::vec3 GetRight() const;

	glm::vec3 GetUp() const;

	void SetActive() const;

private:

	AudioListener(const AudioListener&) = delete;

	AudioListener& operator=(const AudioListener&) = delete;

	AudioListener(AudioListener&&) = delete;

	AudioListener& operator=(AudioListener&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	Listener* listener;

	std::string stringID;
};

#endif // AUDIOLISTENER_H