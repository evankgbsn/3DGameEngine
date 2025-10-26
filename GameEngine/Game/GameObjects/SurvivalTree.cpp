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

}

void SurvivalTree::Initialize()
{
	trunkGraphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("SurvivalTreeTrunk"), TextureManager::GetTexture("SurvivalTree"), TextureManager::GetTexture("SurvivalTree"));
	trunkGraphics->SetShine(32.0f);
	trunkGraphics->SetPosition({ 0.0f, 0.0f, 20.0f });

	branchesGraphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("SurvivalTreeBranches"), TextureManager::GetTexture("SurvivalTree"), TextureManager::GetTexture("SurvivalTree"));
	branchesGraphics->SetShine(32.0f);
	branchesGraphics->SetPosition({ 0.0f, 0.0f, 20.0f });

	leavesGraphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("SurvivalTreeLeaves"), TextureManager::GetTexture("SurvivalTree"), TextureManager::GetTexture("SurvivalTree"));
	leavesGraphics->SetShine(32.0f);
	leavesGraphics->SetPosition({ 0.0f, 0.0f, 20.0f });

	AddComponent(trunkGraphics, "TrunkGraphics");
	AddComponent(branchesGraphics, "BranchesGraphics");
	AddComponent(leavesGraphics, "LeavesGraphics");

	collider = new OrientedBoundingBoxComponent(trunkGraphics->GetModel()->GetVertices());

	AddComponent(collider, "Collider");

	SetupEditorCallbacks();
}

void SurvivalTree::Terminate()
{
	CleanupEditorCallbacks();

	RemoveComponent("Graphics");
	RemoveComponent("Collider");

	delete collider;
	delete trunkGraphics;
	delete branchesGraphics;
	delete leavesGraphics;
}

void SurvivalTree::GameUpdate()
{
	
}

void SurvivalTree::EditorUpdate()
{
	Scene* scene = SceneManager::GetRegisteredScene("SurvivalScene");
	if (scene != nullptr)
	{
		SurvivalTerrain* terrain = dynamic_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));

		if (terrain != nullptr)
		{
			TerrainComponent* terrainComponent = dynamic_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));

			if (terrainComponent != nullptr)
			{
				if (terrainComponent->Loaded())
				{
					glm::vec3 terrainPoint = terrainComponent->GetTerrainPoint(trunkGraphics->GetPosition());
					SetPosition(terrainPoint);
				}
			}
		}
	}
}

void SurvivalTree::Load()
{
	if (!ModelManager::ModelLoaded("SurvivalTreeTrunk"))
	{
		ModelManager::LoadModel("SurvivalTreeTrunk", "Assets/Model/TreeTrunk.gltf", false);
	}

	if (!ModelManager::ModelLoaded("SurvivalTreeBranches"))
	{
		ModelManager::LoadModel("SurvivalTreeBranches", "Assets/Model/TreeBranches.gltf", false);
	}

	if (!ModelManager::ModelLoaded("SurvivalTreeLeaves"))
	{
		ModelManager::LoadModel("SurvivalTreeLeaves", "Assets/Model/TreeLeaves.gltf", false);
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
}

void SurvivalTree::SetPosition(const glm::vec3& newPosition)
{
	trunkGraphics->SetPosition(newPosition);
	branchesGraphics->SetPosition(newPosition);
	leavesGraphics->SetPosition(newPosition);
	collider->UpdateCollider(trunkGraphics->GetTransform());
}

glm::vec3 SurvivalTree::GetPosition() const
{
	return trunkGraphics->GetPosition();
}

void SurvivalTree::SetRotation(const glm::mat4& newRotation)
{
	trunkGraphics->SetRotation(newRotation);
	branchesGraphics->SetRotation(newRotation);
	leavesGraphics->SetRotation(newRotation);
	collider->UpdateCollider(trunkGraphics->GetTransform());
}

glm::mat4 SurvivalTree::GetRotation() const
{
	return trunkGraphics->GetRotation();
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

	trunkGraphics = static_cast<GraphicsObjectTexturedLit*>(GetComponent("TrunkGraphics"));
	branchesGraphics = static_cast<GraphicsObjectTexturedLit*>(GetComponent("BranchesGraphics"));
	leavesGraphics = static_cast<GraphicsObjectTexturedLit*>(GetComponent("LeavesGraphics"));
	collider = static_cast<OrientedBoundingBoxComponent*>(GetComponent("Collider"));

	collider->UpdateCollider(trunkGraphics->GetTransform());

	CleanupEditorCallbacks();
	SetupEditorCallbacks();
}
