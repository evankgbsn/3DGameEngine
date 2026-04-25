#ifndef SOURCE_H
#define SOURCE_H

#include <glm/glm.hpp>

struct IPLSourceSettings;

class Source
{
public:

	Source();

	~Source();

	void SetPosition(const glm::vec3& pos);

	glm::vec3 GetPosition() const;

	void SetRotation(const glm::mat4& rot);

	glm::mat4 GetRotation() const;

	glm::vec3 GetForward() const;

	glm::vec3 GetRight() const;

	glm::vec3 GetUp() const;

private:

	Source(const Source&) = delete;

	Source& operator=(const Source&) = delete;

	Source(Source&&) = delete;

	Source& operator=(Source&&) = delete;

	glm::vec3 position;

	glm::mat4 rotation;
};

#endif // SOURCE_H