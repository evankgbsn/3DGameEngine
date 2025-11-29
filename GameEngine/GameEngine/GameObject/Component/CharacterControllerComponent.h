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

	void SetDisp(const glm::vec3& newDisp);

	glm::vec3 GetPosition() const;

	~CharacterControllerComponent();

private:

	CharacterControllerComponent(const CharacterControllerComponent&) = delete;

	CharacterControllerComponent& operator=(const CharacterControllerComponent&) = delete;

	CharacterControllerComponent(CharacterControllerComponent&&) = delete;

	CharacterControllerComponent& operator=(CharacterControllerComponent&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	CharacterController* controller;

	std::string controllerName;

};

#endif // CHARACTERCONTROLLERCOMPONENT_H