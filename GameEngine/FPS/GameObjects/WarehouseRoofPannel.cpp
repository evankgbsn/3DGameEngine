#include "WarehouseRoofPannel.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

WarehouseRoofPannel::WarehouseRoofPannel() :
	GameObject("WarehouseRoofPannel"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseRoofPannel>(this);
}

WarehouseRoofPannel::~WarehouseRoofPannel()
{
}

void WarehouseRoofPannel::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseRoofPannel"), TextureManager::GetTexture("WarehouseRoofPannel"), TextureManager::GetTexture("WarehouseRoofPannelSpec"));
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseRoofPannel::Terminate()
{
	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseRoofPannel::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseRoofPannel::EditorUpdate()
{
}

void WarehouseRoofPannel::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseRoofPannel"))
	{
		ModelManager::LoadModel("WarehouseRoofPannel", "Assets/Model/WarehouseRoofPannel.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseRoofPannel"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseRoofPannel.png", "WarehouseRoofPannel");
	}

	if (!TextureManager::TextureLoaded("WarehouseRoofPannelSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseRoofPannelSpec");
	}
}

void WarehouseRoofPannel::Unload()
{
}

void WarehouseRoofPannel::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);
}

void WarehouseRoofPannel::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);
}

glm::vec3 WarehouseRoofPannel::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseRoofPannel::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseRoofPannel::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseRoofPannel::Hovered() const
{
	return body->Hovered();
}

void WarehouseRoofPannel::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}
