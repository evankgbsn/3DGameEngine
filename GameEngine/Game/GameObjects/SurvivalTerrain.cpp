#include "SurvivalTerrain.h"

#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Editor/Editor.h"

SurvivalTerrain::SurvivalTerrain() :
	GameObject(std::string("SurvivalTerrain")),
	terrain(nullptr),
	onEditorEnable(nullptr),
	onEditorDisable(nullptr)
{
}

SurvivalTerrain::~SurvivalTerrain()
{
	
}

void SurvivalTerrain::Initialize()
{
	GOLit::Material sandMaterial = GOLit::Material(TextureManager::GetTexture("TerrainSand"), TextureManager::GetTexture("TerrainSand"));
	GOLit::Material rockMaterial = GOLit::Material(TextureManager::GetTexture("TerrainRock"), TextureManager::GetTexture("TerrainRock"));
	GOLit::Material grassMaterial = GOLit::Material(TextureManager::GetTexture("TerrainGrass"), TextureManager::GetTexture("TerrainGrass"));
	GOLit::Material dirtMaterial = GOLit::Material(TextureManager::GetTexture("TerrainDirt"), TextureManager::GetTexture("TerrainDirt"));

	terrain = new TerrainComponent(
		std::string("SurvivalTerrain"),
		std::string("Assets/Texture/Noise.png"),
		{  dirtMaterial, sandMaterial, rockMaterial, grassMaterial },
		std::string("BlendMap"),
		500.0f,
		500.0f,
		500U,
		500U,
		20.0f,
		-10.0f,
		1U
	);

	AddComponent(terrain, "SurvivalTerrain");

	SetupEditorCallbacks();
}

void SurvivalTerrain::Terminate()
{

	CleanupEditorCallbacks();

	if (terrain != nullptr)
	{
		RemoveComponent("SurvivalTerrain");
		delete terrain;
	}
}

void SurvivalTerrain::GameUpdate()
{
}

void SurvivalTerrain::EditorUpdate()
{
}

void SurvivalTerrain::Load()
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
		TextureManager::LoadTexture("Assets/Texture/MyBlendMap.png", "BlendMap");
	}
}

void SurvivalTerrain::Unload()
{
	if (TextureManager::TextureLoaded("TerrainBase"))
	{
		TextureManager::UnloadTexture("TerrainBase");
	}
}

void SurvivalTerrain::Start()
{
	(*onEditorDisable)();
}

void SurvivalTerrain::SetupEditorCallbacks()
{
	onEditorEnable = new std::function<void()>([this]()
		{
			if (!terrain->ColliderVisible())
			{
				terrain->ToggleColliderVisibility();
			}
		});

	onEditorDisable = new std::function<void()>([this]()
		{
			if (terrain->ColliderVisible())
			{
				terrain->ToggleColliderVisibility();
			}
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);
	Editor::RegisterOnEditorDisable(onEditorDisable);
}

void SurvivalTerrain::CleanupEditorCallbacks()
{
	if (onEditorEnable != nullptr)
	{
		Editor::DeregisterOnEditorEnable(onEditorEnable);
		delete onEditorEnable;
		onEditorEnable = nullptr;
	}

	if (onEditorDisable != nullptr)
	{
		Editor::DeregisterOnEditorDisable(onEditorDisable);
		delete onEditorDisable;
		onEditorDisable = nullptr;
	}
}
