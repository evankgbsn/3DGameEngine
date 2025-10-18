#include "SurvivalTree.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/OrientedBoundingBoxComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Camera/Camera.h"
#include "GameEngine/Math/Shapes/LineSegment3D.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "SurvivalTerrain.h"
#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/Editor/Editor.h"
#include "GameEngine/Renderer/Model/Model.h"

SurvivalTree::SurvivalTree() :
	GameObject("SurvivalTree"),
	onEditorEnable(nullptr),
	onEditorDisable(nullptr)
{
	RegisterGameObjectClassType<SurvivalTree>(this);
}

SurvivalTree::~SurvivalTree()
{
	Terminate();
}

void SurvivalTree::Initialize()
{

	while (!ModelManager::ModelLoaded("SurvivalTree"));

	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("SurvivalTree"), TextureManager::GetTexture("SurvivalTree"), TextureManager::GetTexture("SurvivalTree"));
	graphics->SetShine(32.0f);
	graphics->SetPosition({ 0.0f, 0.0f, 0.0f });

	AddComponent(graphics, "Graphics");

	collider = new OrientedBoundingBoxComponent(graphics->GetModel()->GetVertices());

	AddComponent(collider, "Collider");

	SetupEditorCallbacks();
}

void SurvivalTree::Terminate()
{
	CleanupEditorCallbacks();

	RemoveComponent("Graphics");
	RemoveComponent("Collider");

	delete collider;
	delete graphics;
}

void SurvivalTree::GameUpdate()
{
}

void SurvivalTree::EditorUpdate()
{
	Scene* scene = SceneManager::GetRegisteredScene("SurvivalScene");
	if (scene != nullptr)
	{
		SurvivalTerrain* terrain = static_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));

		if (terrain != nullptr)
		{
			TerrainComponent* terrainComponent = static_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));

			if (terrainComponent != nullptr)
			{
				graphics->SetPosition(terrainComponent->GetTerrainPoint(graphics->GetPosition()));
				collider->UpdateCollider(graphics->GetTransform());
			}
		}
	}
}

void SurvivalTree::Load()
{
	if (!ModelManager::ModelLoaded("SurvivalTree"))
	{
		ModelManager::LoadModel("SurvivalTree", "Assets/Model/Tree.gltf", true);
	}

	if (!TextureManager::TextureLoaded("SurvivalTree"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "SurvivalTree");
	}
}

void SurvivalTree::Unload()
{
	if (ModelManager::ModelLoaded("SurvivalTree"))
	{
		ModelManager::UnloadModel("SurvivalTree");
	}

	if (TextureManager::TextureLoaded("SurvivalTree"))
	{
		TextureManager::UnloadTexture("SurvivalTree");
	}
}

void SurvivalTree::Start()
{
	(*onEditorDisable)();

	Scene* scene = SceneManager::GetRegisteredScene("SurvivalScene");
	if (scene != nullptr)
	{
		SurvivalTerrain* terrain = static_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));

		if (terrain != nullptr)
		{
			TerrainComponent* terrainComponent = static_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));

			if (terrainComponent != nullptr)
			{
				graphics->SetPosition(terrainComponent->GetTerrainPoint(graphics->GetPosition()));
				collider->UpdateCollider(graphics->GetTransform());
			}
		}
	}
}

void SurvivalTree::SetPosition(const glm::vec3& newPosition)
{
	graphics->SetPosition(newPosition);
	collider->UpdateCollider(graphics->GetTransform());
}

glm::vec3 SurvivalTree::GetPosition() const
{
	return graphics->GetPosition();
}

void SurvivalTree::SetRotation(const glm::mat4& newRotation)
{
	graphics->SetRotation(newRotation);
	collider->UpdateCollider(graphics->GetTransform());
}

glm::mat4 SurvivalTree::GetRotation() const
{
	return graphics->GetRotation();
}

bool SurvivalTree::Hovered() const
{
	LineSegment3D line = Camera::CastLineFromCursorWithActiveCamera();

	if (collider != nullptr)
	{
		return collider->LineIntersect(line);
	}

	return false;
}

void SurvivalTree::SetupEditorCallbacks()
{
	onEditorEnable = new std::function<void()>([this]()
		{
			if (!collider->IsVisible())
			{
				collider->ToggleVisibility();
			}
		});

	onEditorDisable = new std::function<void()>([this]()
		{
			if (collider->IsVisible())
			{
				collider->ToggleVisibility();
			}
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);
	Editor::RegisterOnEditorDisable(onEditorDisable);
}

void SurvivalTree::CleanupEditorCallbacks()
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

void SurvivalTree::Deserialize()
{
	GameObject::Deserialize();

	graphics = static_cast<GraphicsObjectTexturedLit*>(GetComponent("Graphics"));
	collider = static_cast<OrientedBoundingBoxComponent*>(GetComponent("Collider"));

	collider->UpdateCollider(graphics->GetTransform());

	CleanupEditorCallbacks();
	SetupEditorCallbacks();
}
