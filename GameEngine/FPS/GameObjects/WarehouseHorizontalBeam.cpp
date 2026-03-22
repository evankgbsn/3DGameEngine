#include "WarehouseHorizontalBeam.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseHorizontalBeam::WarehouseHorizontalBeam() :
	GameObject("WarehouseHorizontalBeam"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseHorizontalBeam>(this);
}

WarehouseHorizontalBeam::~WarehouseHorizontalBeam()
{
}

void WarehouseHorizontalBeam::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseHorizontalBeam"), TextureManager::GetTexture("WarehouseHorizontalBeam"), TextureManager::GetTexture("WarehouseHorizontalBeamSpec"));
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseHorizontalBeam::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseHorizontalBeam::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseHorizontalBeam::EditorUpdate()
{
}

void WarehouseHorizontalBeam::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseHorizontalBeam"))
	{
		ModelManager::LoadModel("WarehouseHorizontalBeam", "Assets/Model/WarehouseHorizontalBeam.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseHorizontalBeam"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseHorizontalBeam.png", "WarehouseHorizontalBeam");
	}

	if (!TextureManager::TextureLoaded("WarehouseHorizontalBeamSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseHorizontalBeamSpec");
	}
}

void WarehouseHorizontalBeam::Unload()
{
}

void WarehouseHorizontalBeam::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseHorizontalBeam::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseHorizontalBeam::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseHorizontalBeam::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseHorizontalBeam::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseHorizontalBeam::Hovered() const
{
	return body->Hovered();
}

void WarehouseHorizontalBeam::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}