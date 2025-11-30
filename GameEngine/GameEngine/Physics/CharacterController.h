#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include "PhysicsManager.h"

#include <glm/glm.hpp>

class CharacterController
{
public:

	CharacterController(float radius, float height, const glm::vec3& position);

	void SetRadius(float newRadius);

	void SetHeight(float newHeight);

	float GetRadius() const;

	float GetHeight() const;

	void AddDisp(const glm::vec3& newDisp);

	glm::vec3 GetPosition() const;

	void Move();

	~CharacterController();

private:

	CharacterController() = delete;

	CharacterController(const CharacterController&) = delete;

	CharacterController& operator=(const CharacterController&) = delete;

	CharacterController(CharacterController&&) = delete;

	CharacterController& operator=(CharacterController&&) = delete;

	PxController* controller;

	glm::vec3 disp;

};

#endif // CHARACTERCONTROLLER_H