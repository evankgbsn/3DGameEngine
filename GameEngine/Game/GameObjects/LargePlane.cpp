#include "LargePlane.h"

#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Terrain/Terrain.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

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
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("LargePlane"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Grey"));
	graphics->Translate({ 0.0f, -0.5f, 0.0f });
	graphics->SetShine(32.0f);
	AddComponent(graphics, "Graphics");

	rigidBody = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this);
	AddComponent(rigidBody, "RigidBody");
}

void LargePlane::Terminate()
{
	delete graphics;
}

void LargePlane::GameUpdate()
{
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

const std::vector<char> LargePlane::Serialize() const
{
	return std::vector<char>();
}

void LargePlane::Deserialize(const std::vector<char>& data)
{
}
