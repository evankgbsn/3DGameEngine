#include "Ground.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

Ground::Ground() :
    GameObject("Ground")
{
}

Ground::~Ground()
{
}

void Ground::Initialize()
{
	GOLit::Material sandMaterial = GOLit::Material(TextureManager::GetTexture("TerrainSand"), TextureManager::GetTexture("TerrainSand"));
	GOLit::Material rockMaterial = GOLit::Material(TextureManager::GetTexture("TerrainRock"), TextureManager::GetTexture("TerrainRock"));
	GOLit::Material grassMaterial = GOLit::Material(TextureManager::GetTexture("TerrainGrass"), TextureManager::GetTexture("TerrainGrass"));
	GOLit::Material dirtMaterial = GOLit::Material(TextureManager::GetTexture("TerrainDirt"), TextureManager::GetTexture("TerrainDirt"));

	terrain = new TerrainComponent(
		std::string("Terrain"),
		std::string("Assets/Texture/IslandHeightMap.png"),
		{ dirtMaterial, sandMaterial, rockMaterial, grassMaterial },
		std::string("BlendMap"),
		500.0f,
		500.0f,
		500U,
		500U,
		25.0f,
		-13.0f,
		1U
	);

	AddComponent(terrain, "Terrain");
}

void Ground::Terminate()
{
	if (terrain != nullptr)
	{
		RemoveComponent("SurvivalTerrain");
		delete terrain;
	}
}

void Ground::GameUpdate()
{
}

void Ground::EditorUpdate()
{
	static bool callOnce = false;
	if (terrain->Loaded() && !callOnce)
	{
		body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, terrain->GetModel());
		body->SyncPhysics();
		AddComponent(body, "RigidBody");
		callOnce = true;
	}
}

void Ground::Load()
{
	if (!TextureManager::TextureLoaded("TerrainDirt"))
	{
		TextureManager::LoadTexture("Assets/Texture/RedBrown.png", "TerrainDirt");
	}

	if (!TextureManager::TextureLoaded("TerrainRock"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "TerrainRock");
	}

	if (!TextureManager::TextureLoaded("TerrainGrass"))
	{
		TextureManager::LoadTexture("Assets/Texture/Green.png", "TerrainGrass");
	}

	if (!TextureManager::TextureLoaded("TerrainSand"))
	{
		TextureManager::LoadTexture("Assets/Texture/Beige.png", "TerrainSand");
	}

	if (!TextureManager::TextureLoaded("BlendMap"))
	{
		TextureManager::LoadTexture("Assets/Texture/IslandBlendMap.png", "BlendMap");
	}
}

void Ground::Unload()
{
}

glm::vec3 Ground::GetPosition() const
{
	return terrain->GetPosition();
}

glm::mat4 Ground::GetRotation() const
{
    return terrain->GetRotation();
}

glm::mat4 Ground::GetTransform() const
{
    return terrain->GetTransform();
}

void Ground::SetPosition(const glm::vec3&)
{
}

void Ground::SetRotation(const glm::mat4&)
{
}

bool Ground::Hovered() const
{
    return false;
}
