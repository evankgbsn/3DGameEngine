#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "Component.h"

#include <glm/glm.hpp>

#include <string>
#include <set>

class Source;
class Sound;

class AudioSource : public Component
{
public:

	AudioSource(const glm::vec3& position, const glm::mat4& rotation);

	AudioSource();

	~AudioSource();

	static Sound* LoadSound(const std::string& name, const std::string& path);

	static Sound* GetSound(const std::string& name);

	static bool SoundLoaded(const std::string& name);

	static void UnloadSound(const std::string& name);

	void SetPosition(const glm::vec3& pos);

	glm::vec3 GetPosition() const;

	void SetRotation(const glm::mat4& rot);

	glm::mat4 GetRotation() const;

	glm::vec3 GetForward() const;

	glm::vec3 GetRight() const;

	glm::vec3 GetUp() const;

	void AddSound(const std::string& sound);

	void RemoveSound(const std::string& sound);

	const std::set<std::string>& GetSounds() const;

	void ClearSounds();

	void Play(const std::string& sound);

	void Stop(const std::string& sound);

	void PlayAll();

	void StopAll();

private:

	AudioSource(const AudioSource&) = delete;

	AudioSource& operator=(const AudioSource&) = delete;

	AudioSource(AudioSource&&) = delete;

	AudioSource& operator=(AudioSource&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	Source* source;

};

#endif //AUDIOSOURCE_H