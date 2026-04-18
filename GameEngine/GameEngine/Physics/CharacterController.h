#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include "PhysicsManager.h"

#include <glm/glm.hpp>

#include <functional>

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

	glm::vec3 GetFootPosition() const;

private:

	CharacterController() = delete;

	CharacterController(const CharacterController&) = delete;

	CharacterController& operator=(const CharacterController&) = delete;

	CharacterController(CharacterController&&) = delete;

	CharacterController& operator=(CharacterController&&) = delete;

	PxController* controller;

	PxControllerCollisionFlags collisionFlags;

	glm::vec3 disp;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	GOColored* capsule;

};

#endif // CHARACTERCONTROLLER_H