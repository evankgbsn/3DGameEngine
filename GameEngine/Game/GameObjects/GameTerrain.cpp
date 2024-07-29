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
	terrain = new Terrain("Terrain", "Assets/Texture/Noise.png", std::vector<GOLit::Material>({ {TextureManager::GetTexture("Grass"), TextureManager::GetTexture("Grass")}, {TextureManager::GetTexture("Grass"), TextureManager::GetTexture("Grass")} }), 1000, 1000, 400, 400, 30, -30);
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
		TextureManager::LoadTexture("Assets/Texture/Green.png", "Grass");
	}
}

void GameTerrain::Unload()
{
}
