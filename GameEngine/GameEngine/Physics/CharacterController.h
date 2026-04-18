#ifndef CHARACTERCONTROLLER_H
#define CHARACTERCONTROLLER_H

#include "PhysicsManager.h"

#include <glm/glm.hpp>

#include <functional>
#include <set>

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

	PxControllerCollisionFlags GetCollisionFlags() const;

	~CharacterController();

	void SetOwner(GameObject*);

	GameObject* GetOwner() const;

	void InitializeCapsule();

	glm::vec3 GetFootPosition() const;

	void RegisterCallbackForFixedPhysicsUpdate(std::function<void()>* fixedUpdate);

	void DeregisterCallbackForFixedPhysicsUpdate(std::function<void()>* fixedUpdate);

private:

	friend class PhysicsManager;

	CharacterController() = delete;

	CharacterController(const CharacterController&) = delete;

	CharacterController& operator=(const CharacterController&) = delete;

	CharacterController(CharacterController&&) = delete;

	CharacterController& operator=(CharacterController&&) = delete;

	void FixedUpdate();

	void Move();
	
	PxController* controller;

	PxControllerCollisionFlags collisionFlags;

	glm::vec3 disp;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	GOColored* capsule;

	std::set<std::function<void()>*> fixedUpdates;

};

#endif // CHARACTERCONTROLLER_H