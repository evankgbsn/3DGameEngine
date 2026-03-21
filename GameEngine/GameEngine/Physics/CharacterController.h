#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include "PhysicsManager.h"

#include <glm/glm.hpp>

class GameObject;
class GOColored;

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

	void SetPosition(const glm::vec3& newPosition);

	void Move();

	PxControllerCollisionFlags GetCollisionFlags() const;

	~CharacterController();

	void SetOwner(GameObject*);

	GameObject* GetOwner() const;

	void InitializeCapsule();

private:

	CharacterController() = delete;

	CharacterController(const CharacterController&) = delete;

	CharacterController& operator=(const CharacterController&) = delete;

	CharacterController(CharacterController&&) = delete;

	CharacterController& operator=(CharacterController&&) = delete;

	PxController* controller;

	PxControllerCollisionFlags collisionFlags;

	glm::vec3 disp;

	GOColored* capsule;

};

#endif // CHARACTERCONTROLLER_H