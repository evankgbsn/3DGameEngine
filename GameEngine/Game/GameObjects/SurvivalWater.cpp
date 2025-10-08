#include "SurvivalWater.h"

#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Editor/Editor.h"

SurvivalWater::SurvivalWater() :
	GameObject("SurvivalWater"),
	waterTerrain(nullptr)
{
	RegisterGameObjectClassType<SurvivalWater>(this);
}

SurvivalWater::~SurvivalWater()
{
	Terminate();
}

void SurvivalWater::Initialize()
{
	Texture* waterDiffuse = TextureManager::GetTexture("Water");
	Texture* waterSpecular = TextureManager::GetTexture("Water");

	std::vector<GOLit::Material> materials =
	{
		GOLit::Material(waterDiffuse, waterSpecular),
		GOLit::Material(waterDiffuse, waterSpecular),
		GOLit::Material(waterDiffuse, waterSpecular)
	};

	waterTerrain = new TerrainComponent("WaterTerrain", "Assets/Texture/grey.png", materials, 500.0f, 500.0f, 10U, 10U, 0.0f, -5.2f);

	AddComponent(waterTerrain, "WaterTerrain");

	SetupEditorCallbacks();
}

void SurvivalWater::Terminate()
{
	CleanupEditorCallbacks();

	RemoveComponent("WaterTerrain");

	if (waterTerrain != nullptr)
	{
		delete waterTerrain;
	}
}

void SurvivalWater::GameUpdate()
{
}

void SurvivalWater::EditorUpdate()
{
}

void SurvivalWater::Load()
{
	if (!TextureManager::TextureLoaded("Water"))
	{
		TextureManager::LoadTexture("Assets/Texture/Blue.png", "Water");
	}
}

void SurvivalWater::Unload()
{
	if (TextureManager::TextureLoaded("Water"))
	{
		TextureManager::UnloadTexture("Water");
	}
}

void SurvivalWater::Serialize()
{
	GameObject::Serialize();
}

void SurvivalWater::Deserialize()
{
	GameObject::Deserialize();
}

void SurvivalWater::Start()
{
	(*onEditorDisable)();
}

void SurvivalWater::SetupEditorCallbacks()
{
	onEditorEnable = new std::function<void()>([this]()
		{
			if (waterTerrain != nullptr)
			{
				if (!waterTerrain->ColliderVisible())
				{
					waterTerrain->ToggleColliderVisibility();
				}
			}
		});

	onEditorDisable = new std::function<void()>([this]()
		{
			if (waterTerrain != nullptr)
			{
				if (waterTerrain->ColliderVisible())
				{
					waterTerrain->ToggleColliderVisibility();
				}
			}
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);
	Editor::RegisterOnEditorDisable(onEditorDisable);
}

void SurvivalWater::CleanupEditorCallbacks()
{
	if (onEditorEnable != nullptr)
	{
		Editor::DeregisterOnEditorEnable(onEditorEnable);
		delete onEditorEnable;
	}

	if (onEditorDisable != nullptr)
	{
		Editor::DeregisterOnEditorDisable(onEditorDisable);
		delete onEditorDisable;
	}
}
