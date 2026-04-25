#ifndef LISTENER_H
#define LISTENER_H

#include <glm/glm.hpp>
#include <phonon.h>


class Listener
{
public:

	Listener();

	Listener(const glm::vec3& initialPos, const glm::mat4& initialRot);

	~Listener();

	void SetPosition(const glm::vec3& pos);

	glm::vec3 GetPosition() const;

	void SetRotation(const glm::mat4& rot);

	glm::mat4 GetRotation() const;

	glm::vec3 GetForward() const;

	glm::vec3 GetRight() const;

	glm::vec3 GetUp() const;

	IPLCoordinateSpace3 GetIPLCoordinateSystem() const;

private:

	Listener(const Listener&) = delete;

	Listener& operator=(const Listener&) = delete;

	Listener(Listener&&) = delete;

	Listener& operator=(Listener&&) = delete;

	glm::vec3 position;

	glm::mat4 rotation;

};

#endif // LISTENER_H