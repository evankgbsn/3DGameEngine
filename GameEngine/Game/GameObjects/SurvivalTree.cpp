#include "SurvivalTree.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLitInstanced.h"
#include "GameEngine/GameObject/Component/StaticColliderComponent.h"
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

GraphicsObjectTexturedLitInstanced* SurvivalTree::trunkGraphics = nullptr;
GraphicsObjectTexturedLitInstanced* SurvivalTree::branchesGraphics = nullptr;
GraphicsObjectTexturedLitInstanced* SurvivalTree::leavesGraphics = nullptr;

SurvivalTree::SurvivalTree() :
	GameObject("SurvivalTree"),
	onEditorEnable(nullptr),
	onEditorDisable(nullptr)
{
	RegisterGameObjectClassType<SurvivalTree>(this);
	static unsigned int instanceIDGen = 0;
	instanceID = instanceIDGen;
	instanceIDGen++;
}

SurvivalTree::~SurvivalTree()
{

}

void SurvivalTree::Initialize()
{
	if (trunkGraphics == nullptr)
	{
		trunkGraphics = new GraphicsObjectTexturedLitInstanced("SurvivalTreeTrunk", "SurvivalTree", "SurvivalTree", 1);
		trunkGraphics->SetShine(32.0f);

		branchesGraphics = new GraphicsObjectTexturedLitInstanced("SurvivalTreeBranches", "SurvivalTree", "SurvivalTree", 1);
		branchesGraphics->SetShine(32.0f);

		leavesGraphics = new GraphicsObjectTexturedLitInstanced("SurvivalTreeLeaves", "SurvivalTreeLeaves", "SurvivalTreeLeaves", 1);
		leavesGraphics->SetShine(32.0f);

		instanceID = 0;

		AddComponent(trunkGraphics, "TrunkGraphics");
		AddComponent(branchesGraphics, "BranchesGraphics");
		AddComponent(leavesGraphics, "LeavesGraphics");
	}
	else
	{
		instanceID = trunkGraphics->AddInstance();
		branchesGraphics->AddInstance();
		leavesGraphics->AddInstance();
	}

	collider = new StaticColliderComponent(trunkGraphics, instanceID);
	collider->UpdateCollider();

	AddComponent(collider, "Collider");

	SetupEditorCallbacks();
}

void SurvivalTree::Terminate()
{
	CleanupEditorCallbacks();

	if (trunkGraphics->GetInstanceCount() == 1)
	{
		RemoveComponent("TrunkGraphics");
		RemoveComponent("LeavesGraphics");
		RemoveComponent("BranchesGraphics");

		delete trunkGraphics;
		delete branchesGraphics;
		delete leavesGraphics;
	}

	RemoveComponent("Collider");

	delete collider;
}

void SurvivalTree::GameUpdate()
{
	
}

void SurvivalTree::EditorUpdate()
{
	
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
		TextureManager::LoadTexture("Assets/Texture/DarkBrown.png", "SurvivalTree");
	}

	if (!TextureManager::TextureLoaded("SurvivalTreeLeaves"))
	{
		TextureManager::LoadTexture("Assets/Texture/Green.png", "SurvivalTreeLeaves");
	}
}

void SurvivalTree::Unload()
{
	
}

void SurvivalTree::Start()
{
	(*onEditorDisable)();
}

void SurvivalTree::SetPosition(const glm::vec3& newPosition)
{
	trunkGraphics->SetTranslation(newPosition, instanceID);
	branchesGraphics->SetTranslation(newPosition, instanceID);
	leavesGraphics->SetTranslation(newPosition, instanceID);
	collider->UpdateCollider();
}

glm::vec3 SurvivalTree::GetPosition() const
{
	return trunkGraphics->GetTranslation(instanceID);
}

void SurvivalTree::SetRotation(const glm::mat4& newRotation)
{
	trunkGraphics->SetRotation(newRotation, instanceID);
	branchesGraphics->SetRotation(newRotation, instanceID);
	leavesGraphics->SetRotation(newRotation, instanceID);
	collider->UpdateCollider();
}

glm::mat4 SurvivalTree::GetRotation() const
{
	return trunkGraphics->GetRotation(instanceID);
}

bool SurvivalTree::Hovered() const
{
	LineSegment3D line = Camera::CastLineFromCursorWithActiveCamera();

	if (collider != nullptr)
	{
		return collider->Intersect(line);
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

	trunkGraphics = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("TrunkGraphics"));
	branchesGraphics = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("BranchesGraphics"));
	leavesGraphics = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("LeavesGraphics"));
	collider = static_cast<StaticColliderComponent*>(GetComponent("Collider"));

	collider->SetGraphics(trunkGraphics, instanceID);
	collider->UpdateCollider();

	CleanupEditorCallbacks();
	SetupEditorCallbacks();
}
