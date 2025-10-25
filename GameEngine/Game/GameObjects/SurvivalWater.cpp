#include "SurvivalWater.h"

#include "GameEngine/GameObject/Component/WaterComponent.h"
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

}

void SurvivalWater::Initialize()
{
	waterTerrain = new WaterComponent("WaterTerrain", 500.0f, 500.0f, 500U, 500U, -5.0f);

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
}

void SurvivalWater::Unload()
{
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
