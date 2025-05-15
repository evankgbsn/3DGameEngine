#include "GameTerrain.h"

#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"

GameTerrain::GameTerrain() : 
	GameObject("GameTerrain"),
	terrain(nullptr)
{
	RegisterGameObjectClassType<GameTerrain>(this);
}

GameTerrain::~GameTerrain()
{
}

TerrainComponent* GameTerrain::GetTerrain() const
{
	return terrain;
}

void GameTerrain::Initialize()
{
	terrain = new TerrainComponent("Terrain", "Assets/Texture/Noise.png", std::vector<GOLit::Material>({ {TextureManager::GetTexture("Dirt"), TextureManager::GetTexture("Grey")}, {TextureManager::GetTexture("Dirt"), TextureManager::GetTexture("Grey")} }), 1081, 1081, 500, 500, 30, -15);
	terrain->ToggleCells();
	AddComponent(terrain, "Terrain");
}

void GameTerrain::Terminate()
{
	delete terrain;
}

void GameTerrain::GameUpdate()
{
}

void GameTerrain::EditorUpdate()
{
}

void GameTerrain::Load()
{
	if (!TextureManager::TextureLoaded("Grey"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "Grey");
	}

	if (!TextureManager::TextureLoaded("Grass"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grass.png", "Grass");
	}

	if (!TextureManager::TextureLoaded("Dirt"))
	{
		TextureManager::LoadTexture("Assets/Texture/Dirt.png", "Dirt");
	}
}

void GameTerrain::Unload()
{
}

void GameTerrain::Serialize()
{
	GameObject::Serialize();
}

void GameTerrain::Deserialize()
{
	GameObject::Deserialize();
}
