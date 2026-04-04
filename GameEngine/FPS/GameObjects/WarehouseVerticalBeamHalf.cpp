#include "WarehouseVerticalBeamHalf.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseVerticalBeamHalf::WarehouseVerticalBeamHalf() :
	GameObject("WarehouseVerticalBeamHalf"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseVerticalBeamHalf>(this);
}

WarehouseVerticalBeamHalf::~WarehouseVerticalBeamHalf()
{
}

void WarehouseVerticalBeamHalf::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseVerticalBeamHalf"), "WarehouseVerticalBeamHalf", "WarehouseVerticalBeamHalfSpec", "WarehouseVerticalBeamHalf");
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseVerticalBeamHalf::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseVerticalBeamHalf::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseVerticalBeamHalf::EditorUpdate()
{
}

void WarehouseVerticalBeamHalf::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseVerticalBeamHalf"))
	{
		ModelManager::LoadModel("WarehouseVerticalBeamHalf", "Assets/Model/WarehouseVerticalBeamHalf.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseVerticalBeamHalf"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseVerticalBeamHalf.png", "WarehouseVerticalBeamHalf");
	}

	if (!TextureManager::TextureLoaded("WarehouseVerticalBeamHalfSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseVerticalBeamHalfSpec");
	}
}

void WarehouseVerticalBeamHalf::Unload()
{
}

void WarehouseVerticalBeamHalf::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseVerticalBeamHalf::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseVerticalBeamHalf::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseVerticalBeamHalf::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseVerticalBeamHalf::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseVerticalBeamHalf::Hovered() const
{
	return body->Hovered();
}

void WarehouseVerticalBeamHalf::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}
