#include "ShippingContainer.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

ShippingContainer::ShippingContainer() :
	GameObject("ShippingContainer"),
	body(nullptr),
	graphics(nullptr)
{
	RegisterGameObjectClassType<ShippingContainer>(this);
}

ShippingContainer::~ShippingContainer()
{
}

void ShippingContainer::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(
		ModelManager::GetModel("ShippingContainer"), 
		TextureManager::GetTexture("ShippingContainerRed"), 
		TextureManager::GetTexture("ShippingContainerSpec"));

	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void ShippingContainer::Terminate()
{
	RemoveComponent("RigidBody");

	delete body;

	RemoveComponent("Graphics");

	delete graphics;
}

void ShippingContainer::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void ShippingContainer::EditorUpdate()
{
}

void ShippingContainer::Load()
{
	if (!ModelManager::ModelLoaded("ShippingContainer"))
	{
		ModelManager::LoadModel("ShippingContainer", "Assets/Model/ShippingContainer.gltf", false);
	}

	if (!TextureManager::TextureLoaded("ShippingContainerRed"))
	{
		TextureManager::LoadTexture("Assets/Texture/RedDiffuse.png", "ShippingContainerRed");
	}

	if (!TextureManager::TextureLoaded("ShippingContainerSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/RoughnessMap.png", "ShippingContainerSpec");
	}
}

void ShippingContainer::Unload()
{
}

void ShippingContainer::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

glm::vec3 ShippingContainer::GetPosition() const
{
	return graphics->GetPosition();
}

void ShippingContainer::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::mat4 ShippingContainer::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 ShippingContainer::GetTransform() const
{
	return graphics->GetTransform();
}

bool ShippingContainer::Hovered() const
{
	return body->Hovered();
}

void ShippingContainer::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}
