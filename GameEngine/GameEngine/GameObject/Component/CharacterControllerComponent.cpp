#include "CharacterControllerComponent.h"

#include "../Physics/PhysicsManager.h"
#include "../Physics/CharacterController.h"
#include "../Time/TimeManager.h"

CharacterControllerComponent::CharacterControllerComponent() :
	jumpForce(0.0f)
{
	RegisterComponentClassType<CharacterControllerComponent>(this);
}

CharacterControllerComponent::CharacterControllerComponent(const std::string& name, float radius, float height, const glm::vec3& position) :
	controllerName(name),
	jumpForce(0.0f),
	airTime(0.0f)
{
	RegisterComponentClassType<CharacterControllerComponent>(this);

	controller = PhysicsManager::CreateCharacterController(controllerName, radius, height, position);

}

CharacterControllerComponent::~CharacterControllerComponent()
{
	PhysicsManager::DestroyCharacterController(controllerName);
}

void CharacterControllerComponent::SetRadius(float newRadius)
{
	controller->SetRadius(newRadius);
}

void CharacterControllerComponent::SetHeight(float newHeight)
{
	controller->SetHeight(newHeight);
}

float CharacterControllerComponent::GetRadius() const
{
	return controller->GetRadius();
}

float CharacterControllerComponent::GetHeight() const
{
	return controller->GetHeight();
}

void CharacterControllerComponent::AddDisp(const glm::vec3& newDisp)
{
	controller->AddDisp(newDisp);
}

void CharacterControllerComponent::Jump(float force)
{
	jumpForce = force;
}

bool CharacterControllerComponent::IsFalling() const
{
	return !(controller->GetCollisionFlags() & PxControllerCollisionFlag::eCOLLISION_DOWN);
}

glm::vec3 CharacterControllerComponent::GetPosition() const
{
	return controller->GetPosition();
}

glm::vec3 CharacterControllerComponent::GetFootPosition() const
{
	return controller->GetFootPosition();
}

void CharacterControllerComponent::SetOwner(GameObject* owner)
{
	controller->SetOwner(owner);
}

void CharacterControllerComponent::SetPosition(const glm::vec3& newPosition)
{
	controller->SetPosition(newPosition);
}

void CharacterControllerComponent::Serialize()
{
}

void CharacterControllerComponent::Deserialize()
{
}

void CharacterControllerComponent::Update()
{
	float fixedDt = 1.0f / 240.0f; // Match PhysicsManager::stepSize

	if (IsFalling()) {
		jumpForce += -60.0f * fixedDt;
	}
	else if (jumpForce < 0.0f) {
		jumpForce = -2.0f;
	}

	// Apply Velocity to Position (Displacement = Velocity * Time)
	float verticalDisplacement = jumpForce * fixedDt;

	controller->AddDisp(glm::vec3(0.0f, verticalDisplacement, 0.0f));
}
