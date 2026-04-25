#include "WarehouseBrickWall.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Audio/AudioManager.h"
#include "GameEngine/Audio/AudioObject.h"

WarehouseBrickWall::WarehouseBrickWall() :
	GameObject("WarehouseBrickWall"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<WarehouseBrickWall>(this);
}

WarehouseBrickWall::~WarehouseBrickWall()
{
}

void WarehouseBrickWall::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("WarehouseBrickWall"), "WarehouseBrickWall", "WarehouseBrickWallSpec", "WarehouseBrickWallNormal");
	graphics->SetShine(4.0f);

	AddComponent(graphics, "Graphics");

	AudioObject::Material mat =
	{
		0.03f, 0.04f, 0.07f,
		0.20f,
		0.01f, 0.01f, 0.01f
	};

	audio = AudioManager::CreateAudioObject(GetName(), graphics->GetModel(), graphics->GetPosition(), graphics->GetRotation(), mat);

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void WarehouseBrickWall::Terminate()
{
	AudioManager::Delete(audio);

	RemoveComponent("RigidBody");
	delete body;

	RemoveComponent("Graphics");
	delete graphics;
}

void WarehouseBrickWall::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void WarehouseBrickWall::EditorUpdate()
{
}

void WarehouseBrickWall::Load()
{
	if (!ModelManager::ModelLoaded("WarehouseBrickWall"))
	{
		ModelManager::LoadModel("WarehouseBrickWall", "Assets/Model/WarehouseBrickWall.gltf", false);
	}

	if (!TextureManager::TextureLoaded("WarehouseBrickWall"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseBrickWall.png", "WarehouseBrickWall");
	}

	if (!TextureManager::TextureLoaded("WarehouseBrickWallSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "WarehouseBrickWallSpec");
	}

	if (!TextureManager::TextureLoaded("WarehouseBrickWallNormal"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseBrickWallNormal.png", "WarehouseBrickWallNormal");
	}
}

void WarehouseBrickWall::Unload()
{
}

void WarehouseBrickWall::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
	body->SetPosition(pos);audio->SetPosition(pos);
}

void WarehouseBrickWall::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
	body->SetRotation(rot);audio->SetRotation(rot);
}

glm::vec3 WarehouseBrickWall::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 WarehouseBrickWall::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 WarehouseBrickWall::GetTransform() const
{
	return graphics->GetTransform();
}

bool WarehouseBrickWall::Hovered() const
{
	return body->Hovered();
}

void WarehouseBrickWall::Deserialize()
{
	GameObject::Deserialize();
	body->SetOwner(this);
}
