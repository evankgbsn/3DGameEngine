#include "WarehouseVerticalBeamCorner.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseVerticalBeamCorner::WarehouseVerticalBeamCorner() :
	GameObject("WarehouseVerticalBeamCorner"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseVerticalBeamCorner>(this);
}

WarehouseVerticalBeamCorner::~WarehouseVerticalBeamCorner()
{
}

void WarehouseVerticalBeamCorner::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseVerticalBeamCorner"), "WarehouseVerticalBeamCorner", "WarehouseVerticalBeamCornerSpec", "WarehouseVerticalBeamCorner");
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseVerticalBeamCorner::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseVerticalBeamCorner::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseVerticalBeamCorner::EditorUpdate()
{
}

void WarehouseVerticalBeamCorner::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseVerticalBeamCorner"))
	{
		ModelManager::LoadModel("WarehouseVerticalBeamCorner", "Assets/Model/WarehouseVerticalBeamCorner.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseVerticalBeamCorner"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseVerticalBeamCorner.png", "WarehouseVerticalBeamCorner");
	}

	if (!TextureManager::TextureLoaded("WarehouseVerticalBeamCornerSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseVerticalBeamCornerSpec");
	}
}

void WarehouseVerticalBeamCorner::Unload()
{
}

void WarehouseVerticalBeamCorner::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseVerticalBeamCorner::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseVerticalBeamCorner::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseVerticalBeamCorner::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseVerticalBeamCorner::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseVerticalBeamCorner::Hovered() const
{
	return body->Hovered();
}

void WarehouseVerticalBeamCorner::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}