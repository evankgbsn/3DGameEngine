#include "WarehouseDoorway.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"

WarehouseDoorway::WarehouseDoorway() :
	GameObject("WarehouseDoorway"),
	doorwayGraphics(nullptr),
	doorwayBody(nullptr)
{
	RegisterGameObjectClassType<WarehouseDoorway>(this);
}

WarehouseDoorway::~WarehouseDoorway()
{
}

void WarehouseDoorway::Initialize()
{
	doorwayGraphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseDoorway"), "WarehouseDoorway", "WarehouseDoorwaySpec", "WarehouseDoorway");
	doorwayGraphics->SetShine(4.0f);

	AddComponent(doorwayGraphics, "DoorwayGraphics");

	doorwayBody = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, doorwayGraphics->GetModel());
	doorwayBody->SyncPhysics();

	AddComponent(doorwayBody, "RigidBody");
}

void WarehouseDoorway::Terminate()
{
	RemoveComponent("RigidBody");
	delete doorwayBody;

	RemoveComponent("DoorwayGraphics");
	delete doorwayGraphics;
}

void WarehouseDoorway::GameUpdate()
{
	doorwayBody->SyncPhysics();
	doorwayBody->Update();
}

void WarehouseDoorway::EditorUpdate()
{
}

void WarehouseDoorway::Load()
{
	if (!TextureManager::TextureLoaded("WarehouseDoorway"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseDoorway.png", "WarehouseDoorway");
	}

	if (!TextureManager::TextureLoaded("WarehouseDoorwaySpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseDoorwaySpec");
	}

	if (!ModelManager::ModelLoaded("WarehouseDoorway"))
	{
		ModelManager::LoadModel("WarehouseDoorway", "Assets/Model/WarehouseDoorway.gltf", false);
	}
}

void WarehouseDoorway::Unload()
{
}

void WarehouseDoorway::SetPosition(const glm::vec3& pos)
{
	doorwayGraphics->SetPosition(pos);
	doorwayBody->SetPosition(pos);
}

void WarehouseDoorway::SetRotation(const glm::mat4& rot)
{
	doorwayGraphics->SetRotation(rot);
	doorwayBody->SetRotation(rot);
}

glm::vec3 WarehouseDoorway::GetPosition() const
{
	return doorwayGraphics->GetPosition();
}

glm::mat4 WarehouseDoorway::GetRotation() const
{
	return doorwayGraphics->GetRotation();
}

glm::mat4 WarehouseDoorway::GetTransform() const
{
	return doorwayGraphics->GetTransform();
}

bool WarehouseDoorway::Hovered() const
{
	return doorwayBody->Hovered();
}

void WarehouseDoorway::Deserialize()
{
	GameObject::Deserialize();
	doorwayBody->SetOwner(this);
}
