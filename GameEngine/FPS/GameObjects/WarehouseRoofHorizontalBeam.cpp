#include "WarehouseRoofHorizontalBeam.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseRoofHorizontalBeam::WarehouseRoofHorizontalBeam() :
	GameObject("WarehouseRoofHorizontalBeam"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseRoofHorizontalBeam>(this);
}

WarehouseRoofHorizontalBeam::~WarehouseRoofHorizontalBeam()
{
}

void WarehouseRoofHorizontalBeam::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseRoofHorizontalBeam"), "WarehouseRoofHorizontalBeam", "WarehouseRoofHorizontalBeamSpec", "WarehouseRoofHorizontalBeamNormal");
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseRoofHorizontalBeam::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseRoofHorizontalBeam::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseRoofHorizontalBeam::EditorUpdate()
{
}

void WarehouseRoofHorizontalBeam::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseRoofHorizontalBeam"))
	{
		ModelManager::LoadModel("WarehouseRoofHorizontalBeam", "Assets/Model/WarehouseRoofHorizontalBeam.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseRoofHorizontalBeam"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseRoofHorizontalBeam.png", "WarehouseRoofHorizontalBeam");
	}

	if (!TextureManager::TextureLoaded("WarehouseRoofHorizontalBeamSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseRoofHorizontalBeamSpec");
	}

	if (!TextureManager::TextureLoaded("WarehouseRoofHorizontalBeamNormal"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseRoofHorizontalBeamNormal.png", "WarehouseRoofHorizontalBeamNormal");
	}
}

void WarehouseRoofHorizontalBeam::Unload()
{
}

void WarehouseRoofHorizontalBeam::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseRoofHorizontalBeam::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseRoofHorizontalBeam::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseRoofHorizontalBeam::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseRoofHorizontalBeam::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseRoofHorizontalBeam::Hovered() const
{
	return body->Hovered();
}

void WarehouseRoofHorizontalBeam::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}
