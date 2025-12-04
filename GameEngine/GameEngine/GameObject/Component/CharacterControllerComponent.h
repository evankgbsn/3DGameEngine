#ifndef CHARACTERCONTROLLERCOMPONENT_H
#define CHARACTERCONTROLLERCOMPONENT_H

#include "Component.h"
#include <string>

class CharacterController;

class CharacterControllerComponent : public Component
{
public:

	CharacterControllerComponent();

	CharacterControllerComponent(const std::string& name, float radius, float height, const glm::vec3& position);

	void SetRadius(float newRadius);

	void SetHeight(float newHeight);

	float GetRadius() const;

	float GetHeight() const;

	void AddDisp(const glm::vec3& newDisp);

	void Jump(float force, float airTime);

	bool IsFalling() const;

	glm::vec3 GetPosition() const;

	~CharacterControllerComponent();

	void Update() override;

private:

	CharacterControllerComponent(const CharacterControllerComponent&) = delete;

	CharacterControllerComponent& operator=(const CharacterControllerComponent&) = delete;

	CharacterControllerComponent(CharacterControllerComponent&&) = delete;

	CharacterControllerComponent& operator=(CharacterControllerComponent&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	CharacterController* controller;

	std::string controllerName;

	float jumpForce;

	float airTime;

};

#endif // CHARACTERCONTROLLERCOMPONENT_H