#include "WarehouseWindowWall.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseWindowWall::WarehouseWindowWall() :
	GameObject("WarehouseWindowWall"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseWindowWall>(this);
}

WarehouseWindowWall::~WarehouseWindowWall()
{
}

void WarehouseWindowWall::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseWindowWall"), "WarehouseWindowWall", "WarehouseWindowWallSpec", "WarehouseWindowWallNormal");
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseWindowWall::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseWindowWall::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseWindowWall::EditorUpdate()
{
}

void WarehouseWindowWall::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseWindowWall"))
	{
		ModelManager::LoadModel("WarehouseWindowWall", "Assets/Model/WarehouseWindowWall.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseWindowWall"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseWindowWall.png", "WarehouseWindowWall");
	}

	if(!TextureManager::TextureLoaded("WarehouseWindowWallSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseWindowWallSpec");
	}

	if (!TextureManager::TextureLoaded("WarehouseWindowWallNormal"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseWindowWallNormal.png", "WarehouseWindowWallNormal");
	}
}

void WarehouseWindowWall::Unload()
{
}

void WarehouseWindowWall::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseWindowWall::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseWindowWall::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseWindowWall::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseWindowWall::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseWindowWall::Hovered() const
{
	return body->Hovered();
}

void WarehouseWindowWall::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}
