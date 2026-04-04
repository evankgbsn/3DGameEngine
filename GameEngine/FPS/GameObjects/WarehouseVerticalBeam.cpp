#include "WarehouseVerticalBeam.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseVerticalBeam::WarehouseVerticalBeam() :
	GameObject("WarehouseVerticalBeam"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseVerticalBeam>(this);
}

WarehouseVerticalBeam::~WarehouseVerticalBeam()
{
}

void WarehouseVerticalBeam::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseVerticalBeam"), "WarehouseVerticalBeam", "WarehouseVerticalBeamSpec", "WarehouseVerticalBeam");
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseVerticalBeam::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseVerticalBeam::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseVerticalBeam::EditorUpdate()
{
}

void WarehouseVerticalBeam::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseVerticalBeam"))
	{
		ModelManager::LoadModel("WarehouseVerticalBeam", "Assets/Model/WarehouseVerticalBeam.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseVerticalBeam"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseVerticalBeam.png", "WarehouseVerticalBeam");
	}

	if (!TextureManager::TextureLoaded("WarehouseVerticalBeamSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseVerticalBeamSpec");
	}
}

void WarehouseVerticalBeam::Unload()
{
}

void WarehouseVerticalBeam::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseVerticalBeam::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseVerticalBeam::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseVerticalBeam::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseVerticalBeam::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseVerticalBeam::Hovered() const
{
	return body->Hovered();
}

void WarehouseVerticalBeam::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}