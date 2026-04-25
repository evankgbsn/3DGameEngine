#include "Asteroid.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

Asteroid::Asteroid() :
	GameObject("Asteroid"),
	graphics(nullptr),
	body(nullptr)
{
	RegisterGameObjectClassType<Asteroid>(this);
}

Asteroid::~Asteroid()
{
}

void Asteroid::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("Asteroid"), "Asteroid", "AsteroidSpec", "AsteroidNorm");
	graphics->SetShine(4.0f);
	graphics->SetPosition({ 0.0f, 0.0f, 0.0f });

	AddComponent(graphics, "Graphics");

	body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	body->SyncPhysics();

	AddComponent(body, "RigidBody");
}

void Asteroid::Terminate()
{
	RemoveComponent("Graphics");
	delete graphics;

	RemoveComponent("RigidBody");
	delete body;
}

void Asteroid::GameUpdate()
{
	body->SyncPhysics();
	body->Update();
}

void Asteroid::EditorUpdate()
{
}

void Asteroid::Load()
{
	if (!ModelManager::ModelLoaded("Asteroid"))
	{
		ModelManager::LoadModel("Asteroid", "Assets/Model/Asteroid.gltf", false);
	}

	if (!TextureManager::TextureLoaded("Asteroid"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "Asteroid");
	}

	if (!TextureManager::TextureLoaded("AsteroidSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "AsteroidSpec");
	}

	if (!TextureManager::TextureLoaded("AsteroidNorm"))
	{
		TextureManager::LoadTexture("Assets/Texture/Normal.png", "AsteroidNorm");
	}
}

void Asteroid::Unload()
{
}

bool Asteroid::Hovered() const
{
	return body->Hovered();
}

glm::vec3 Asteroid::GetPosition() const
{
	return graphics->GetPosition();
}

void Asteroid::SetPosition(const glm::vec3& newPos)
{
	body->SetPosition(newPos);
	graphics->SetPosition(newPos);
}

glm::mat4 Asteroid::GetRotation() const
{
	return graphics->GetRotation();
}

void Asteroid::SetRotation(const glm::mat4& newRot)
{
	body->SetRotation(newRot);
	graphics->SetRotation(newRot);
}

glm::mat4 Asteroid::GetTransform() const
{
	return graphics->GetTransform();
}
