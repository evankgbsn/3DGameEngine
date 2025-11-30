#include "CharacterControllerComponent.h"

#include "../Physics/PhysicsManager.h"
#include "../Physics/CharacterController.h"

CharacterControllerComponent::CharacterControllerComponent()
{
	RegisterComponentClassType<CharacterControllerComponent>(this);
}

CharacterControllerComponent::CharacterControllerComponent(const std::string& name, float radius, float height, const glm::vec3& position) :
	controllerName(name)
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
}
