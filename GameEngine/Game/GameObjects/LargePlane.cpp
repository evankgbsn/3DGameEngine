#include "LargePlane.h"

#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTextured.h"
#include "GameEngine/Terrain/Terrain.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects//GOTerrain.h"

LargePlane::LargePlane() :
	GameObject("LargePlane"),
	graphics(nullptr)
{
	RegisterGameObjectClassType<LargePlane>(this);
}

LargePlane::~LargePlane()
{
}

void LargePlane::Initialize()
{
	//graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("LargePlane"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Grey"));
	//graphics->Translate({ 0.0f, -0.5f, 0.0f });
	//graphics->SetShine(32.0f);
	//AddComponent(graphics, "Graphics");

	Model* terrainModel = ModelManager::CreateModelTerrain("PhysxTest", "Assets/Texture/Noise.png", 1081, 1081, 200, 200, 30, -15);

	graphics = new GraphicsObjectTexturedLit(terrainModel, TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Grey"));
	graphics->SetShine(8.0f);
	AddComponent(graphics, "TerrainGraphics");

	skybox = new GraphicsObjectTextured(ModelManager::LoadModel("Skybox", "Assets/Model/Skybox.gltf"), TextureManager::LoadTexture("Assets/Texture/Skybox2.png", "Skybox"));
	skybox->SetScale({ 1000.0f, 1000.0f, 1000.0f});
	AddComponent(skybox, "SkyboxGraphics");


	rigidBody = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());
	AddComponent(rigidBody, "RigidBody");

	rigidBody->SyncPhysics();
}

void LargePlane::Terminate()
{
	delete graphics;
	delete skybox;
	delete rigidBody;
}

void LargePlane::GameUpdate()
{
	rigidBody->SyncPhysics();
}

void LargePlane::EditorUpdate()
{
}

void LargePlane::Load()
{
	if (!TextureManager::TextureLoaded("Grey"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "Grey");
	}

	if (!ModelManager::ModelLoaded("LargePlane"))
	{
		ModelManager::LoadModel("LargePlane", "Assets/Model/LargePlane.gltf");
	}
}

void LargePlane::Unload()
{
	if (TextureManager::TextureLoaded("Grey"))
	{
		TextureManager::UnloadTexture("Grey");
	}

	if (ModelManager::ModelLoaded("LargePlane"))
	{
		ModelManager::UnloadModel("LargePlane");
	}
}

void LargePlane::SetPosition(const glm::vec3& newPosition)
{
	graphics->SetPosition(newPosition);
}

void LargePlane::SetRotation(const glm::mat4& newRotation)
{
	graphics->SetRotation(newRotation);
}

glm::vec3 LargePlane::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 LargePlane::GetRotation() const
{
	return graphics->GetRotation();
}

glm::mat4 LargePlane::GetTransform() const
{
	return graphics->GetTransform();
}

void LargePlane::Serialize()
{
	GameObject::Serialize();
}

void LargePlane::Deserialize()
{
	GameObject::Deserialize();
}
