#include "GameTerrain.h"

#include "GameEngine/Terrain/Terrain.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"

GameTerrain::GameTerrain() :
	terrain(nullptr)
{
}

GameTerrain::~GameTerrain()
{
}

Terrain* GameTerrain::GetTerrain() const
{
	return terrain;
}

void GameTerrain::Initialize()
{
	terrain = new Terrain("Terrain", "Assets/Texture/Noise.png", std::vector<GOLit::Material>({ {TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Grey")}, {TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Grey")} }), 1081, 1081, 400, 400, 10, -10);
	//terrain->ToggleCells();

	
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
