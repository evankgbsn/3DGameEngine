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
	GOLit::Material baseMaterial = GOLit::Material(TextureManager::GetTexture("TerrainBase"), TextureManager::GetTexture("TerrainBase"));

	terrain = new TerrainComponent(
		std::string("SurvivalTerrain"),
		std::string("Assets/Texture/Noise.png"),
		{ baseMaterial, baseMaterial, baseMaterial},
		500.0f,
		500.0f,
		500U,
		500U,
		20.0f,
		-10.0f
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
	if (!TextureManager::TextureLoaded("TerrainBase"))
	{
		TextureManager::LoadTexture("Assets/Texture/Green.png", "TerrainBase");
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
