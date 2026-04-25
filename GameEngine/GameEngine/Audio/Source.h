#ifndef SOURCE_H
#define SOURCE_H

#include <glm/glm.hpp>

#include <string>
#include <set>

class Source
{
public:

	Source();

	Source(const std::string& name, const glm::vec3& position, const glm::mat4& rotation);

	~Source();

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

	Source(const Source&) = delete;

	Source& operator=(const Source&) = delete;

	Source(Source&&) = delete;

	Source& operator=(Source&&) = delete;

	glm::vec3 position;

	glm::mat4 rotation;

	std::set<std::string> sounds;

	const std::string name;
};

#endif // SOURCE_H