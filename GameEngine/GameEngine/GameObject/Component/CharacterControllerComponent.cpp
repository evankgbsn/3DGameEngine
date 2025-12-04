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
	jumpForce(0.0f)
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

void CharacterControllerComponent::Serialize()
{
}

void CharacterControllerComponent::Deserialize()
{
}

void CharacterControllerComponent::Update()
{
	static float airTime = 1.0f;

	if (airTime <= 0.0f)
	{
		airTime = 0;
	}
	else
	{
		airTime -= TimeManager::DeltaTime();
		controller->AddDisp(glm::vec3(0.0f, jumpForce * airTime * TimeManager::DeltaTime(), 0.0f));
	}
}
