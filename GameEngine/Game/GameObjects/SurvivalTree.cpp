#include "SurvivalTree.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLitInstanced.h"
#include "GameEngine/GameObject/Component/StaticColliderComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Editor/Editor.h"
#include "GameEngine/Scene/Scene.h"

GraphicsObjectTexturedLitInstanced* SurvivalTree::trunk = nullptr;
GraphicsObjectTexturedLitInstanced* SurvivalTree::branches = nullptr;
GraphicsObjectTexturedLitInstanced* SurvivalTree::leaves = nullptr;

std::list<std::function<void()>*> SurvivalTree::onGraphicsDeserializedFunctions = std::list<std::function<void()>*>();

unsigned int SurvivalTree::instanceIDGen = 0;

SurvivalTree::SurvivalTree() :
	GameObject("SurvivalTree")
{
	RegisterGameObjectClassType<SurvivalTree>(this);
}

SurvivalTree::~SurvivalTree()
{
}

void SurvivalTree::Initialize()
{
	if (!GetOwningScene()->IsDeserializing())
	{
		if (trunk == nullptr)
		{
			instanceID = 0;

			trunk = new GraphicsObjectTexturedLitInstanced("SurvivalTreeTrunk", "SurvivalTree", "SurvivalTree", 1);
			trunk->SetShine(32.0f);
			branches = new GraphicsObjectTexturedLitInstanced("SurvivalTreeBranches", "SurvivalTree", "SurvivalTree", 1);
			branches->SetShine(32.0f);
			leaves = new GraphicsObjectTexturedLitInstanced("SurvivalTreeLeaves", "SurvivalTreeLeaves", "SurvivalTreeLeaves", 1);
			leaves->SetShine(32.0f);

			AddComponent(trunk, "TrunkGraphics");
			AddComponent(branches, "BranchesGraphics");
			AddComponent(leaves, "LeavesGraphics");
		}
		else
		{
			instanceID = trunk->AddInstance();
			branches->AddInstance();
			leaves->AddInstance();
			trunk->FinalizeTransforms();
			branches->FinalizeTransforms();
			leaves->FinalizeTransforms();
		}

		collider = new StaticColliderComponent(trunk, instanceID);
		collider->UpdateCollider();
		AddComponent(collider, "Collider");
	}
	else
	{

	}

	SetupEditorCallbacks();
}

void SurvivalTree::Terminate()
{
	CleanupEditorCallbacks();

	if (HasComponent("TrunkGraphics"))
	{
		RemoveComponent("TrunkGraphics");
		RemoveComponent("BranchesGraphics");
		RemoveComponent("LeavesGraphics");

		delete trunk;
		trunk = nullptr;
		delete branches;
		branches = nullptr;
		delete leaves;
		leaves = nullptr;
	}
	else
	{
		if (trunk != nullptr)
		{
			trunk->RemoveInstanceByID(instanceID);
			branches->RemoveInstanceByID(instanceID);
			leaves->RemoveInstanceByID(instanceID);
		}
	}

	if (HasComponent("Collider"))
	{
		RemoveComponent("Collider");
		delete collider;
	}
}

void SurvivalTree::GameUpdate()
{
	collider->UpdateCollider();
}

void SurvivalTree::EditorUpdate()
{
	collider->UpdateCollider();
}

void SurvivalTree::Load()
{
	if (!ModelManager::ModelLoaded("SurvivalTreeBranches"))
	{
		ModelManager::LoadModel("SurvivalTreeBranches", "Assets/Model/TreeBranches.gltf", false);
	}

	if (!ModelManager::ModelLoaded("SurvivalTreeLeaves"))
	{
		ModelManager::LoadModel("SurvivalTreeLeaves", "Assets/Model/TreeLeaves.gltf", false);
	}

	if (!ModelManager::ModelLoaded("SurvivalTreeTrunk"))
	{
		ModelManager::LoadModel("SurvivalTreeTrunk", "Assets/Model/TreeTrunk.gltf", false);
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

void SurvivalTree::Serialize()
{
	GameObject::Serialize();
}

void SurvivalTree::Deserialize()
{
	GameObject::Deserialize();

	if (HasComponent("TrunkGraphics"))
	{
		trunk = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("TrunkGraphics"));
		branches = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("BranchesGraphics"));
		leaves = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("LeavesGraphics"));

		instanceIDGen = 0;

		for (const auto& function : onGraphicsDeserializedFunctions)
		{
			(*function)();
			delete function;
		}

		onGraphicsDeserializedFunctions.clear();
	}
	
	std::function<void()>* initCollider = new std::function<void()>([this]()
		{
			instanceID = instanceIDGen++;

			collider = static_cast<StaticColliderComponent*>(GetComponent("Collider"));

			collider->SetGraphics(trunk, instanceID);
			collider->UpdateCollider();
		});

	if (trunk != nullptr)
	{
		(*initCollider)();
		delete initCollider;
	}
	else
	{
		onGraphicsDeserializedFunctions.push_back(initCollider);
	}

	CleanupEditorCallbacks();
	SetupEditorCallbacks();
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

void SurvivalTree::SetPosition(const glm::vec3& newPos)
{
	trunk->SetTranslation(newPos, instanceID);
	branches->SetTranslation(newPos, instanceID);
	leaves->SetTranslation(newPos, instanceID);
	trunk->UpdateInstanceByID(instanceID);
	branches->UpdateInstanceByID(instanceID);
	leaves->UpdateInstanceByID(instanceID);
	collider->UpdateCollider();
}

glm::vec3 SurvivalTree::GetPosition() const
{
	return trunk->GetTranslation(instanceID);
}

void SurvivalTree::SetRotation(const glm::mat4& newRot)
{
	trunk->SetRotation(newRot, instanceID);
	branches->SetRotation(newRot, instanceID);
	leaves->SetRotation(newRot, instanceID);
	trunk->UpdateInstanceByID(instanceID);
	branches->UpdateInstanceByID(instanceID);
	leaves->UpdateInstanceByID(instanceID);
	collider->UpdateCollider();
}

glm::mat4 SurvivalTree::GetRotation() const
{
	return trunk->GetRotation(instanceID);
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

