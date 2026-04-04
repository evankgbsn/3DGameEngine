#include "WarehouseRoofWall.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseRoofWall::WarehouseRoofWall() :
	GameObject("WarehouseRoofWall"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseRoofWall>(this);
}

WarehouseRoofWall::~WarehouseRoofWall()
{
}

void WarehouseRoofWall::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseRoofWall"), "WarehouseRoofWall", "WarehouseRoofWallSpec", "WarehouseRoofWall");
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseRoofWall::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseRoofWall::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseRoofWall::EditorUpdate()
{
}

void WarehouseRoofWall::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseRoofWall"))
	{
		ModelManager::LoadModel("WarehouseRoofWall", "Assets/Model/WarehouseRoofWall.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseRoofWall"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseRoofWall.png", "WarehouseRoofWall");
	}

	if (!TextureManager::TextureLoaded("WarehouseRoofWallSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseRoofWallSpec");
	}
}

void WarehouseRoofWall::Unload()
{
}

void WarehouseRoofWall::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseRoofWall::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseRoofWall::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseRoofWall::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseRoofWall::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseRoofWall::Hovered() const
{
	return body->Hovered();
}

void WarehouseRoofWall::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}
