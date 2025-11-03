#include "SurvivalRockLarge.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLitInstanced.h"
#include "GameEngine/GameObject/Component/StaticColliderComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Editor/Editor.h"
#include "GameEngine/Scene/Scene.h"

GraphicsObjectTexturedLitInstanced* SurvivalRockLarge::rock = nullptr;

std::list<std::function<void()>*> SurvivalRockLarge::onGraphicsDeserializedFunctions = std::list<std::function<void()>*>();

unsigned int SurvivalRockLarge::instanceIDGen = 0;

SurvivalRockLarge::SurvivalRockLarge() :
	GameObject("SurvivalRockLarge")
{
	RegisterGameObjectClassType<SurvivalRockLarge>(this);
}

SurvivalRockLarge::~SurvivalRockLarge()
{
}

void SurvivalRockLarge::Initialize()
{
	if (!GetOwningScene()->IsDeserializing())
	{
		if (rock == nullptr)
		{
			instanceID = 0;

			rock = new GraphicsObjectTexturedLitInstanced("SurvivalRockLarge", "SurvivalRockLarge", "SurvivalRockLarge", 1);
			rock->SetShine(32.0f);

			AddComponent(rock, "RockGraphics");
		}
		else
		{
			instanceID = rock->AddInstance();
			rock->FinalizeTransforms();
		}

		collider = new StaticColliderComponent(rock, instanceID);
		collider->UpdateCollider();
		AddComponent(collider, "Collider");
	}
	else
	{

	}

	SetupEditorCallbacks();
}

void SurvivalRockLarge::Terminate()
{
	CleanupEditorCallbacks();

	if (HasComponent("RockGraphics"))
	{
		RemoveComponent("RockGraphics");

		delete rock;
		rock = nullptr;
	}
	else
	{
		if (rock != nullptr)
		{
			rock->RemoveInstanceByID(instanceID);
		}
	}

	if (HasComponent("Collider"))
	{
		RemoveComponent("Collider");
		delete collider;
	}
}

void SurvivalRockLarge::GameUpdate()
{
	collider->UpdateCollider();
}

void SurvivalRockLarge::EditorUpdate()
{
	collider->UpdateCollider();
}

void SurvivalRockLarge::Load()
{
	if (!ModelManager::ModelLoaded("SurvivalRockLarge"))
	{
		ModelManager::LoadModel("SurvivalRockLarge", "Assets/Model/RockLarge.gltf", false);
	}

	if (!TextureManager::TextureLoaded("SurvivalRockLarge"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "SurvivalRockLarge");
	}
}

void SurvivalRockLarge::Unload()
{
}

void SurvivalRockLarge::Start()
{
	(*onEditorDisable)();
}

void SurvivalRockLarge::Serialize()
{
	GameObject::Serialize();
}

void SurvivalRockLarge::Deserialize()
{
	GameObject::Deserialize();

	if (HasComponent("RockGraphics"))
	{
		rock = dynamic_cast<GraphicsObjectTexturedLitInstanced*>(GetComponent("RockGraphics"));

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

			collider->SetGraphics(rock, instanceID);
			collider->UpdateCollider();
		});

	if (rock != nullptr)
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

bool SurvivalRockLarge::Hovered() const
{
	LineSegment3D line = Camera::CastLineFromCursorWithActiveCamera();

	if (collider != nullptr)
	{
		return collider->Intersect(line);
	}

	return false;
}

void SurvivalRockLarge::SetPosition(const glm::vec3& newPos)
{
	rock->SetTranslation(newPos, instanceID);
	rock->UpdateInstanceByID(instanceID);
	collider->UpdateCollider();
}

glm::vec3 SurvivalRockLarge::GetPosition() const
{
	return rock->GetTranslation(instanceID);
}

void SurvivalRockLarge::SetRotation(const glm::mat4& newRot)
{
	rock->SetRotation(newRot, instanceID);
	rock->UpdateInstanceByID(instanceID);
	collider->UpdateCollider();
}

glm::mat4 SurvivalRockLarge::GetRotation() const
{
	return rock->GetRotation(instanceID);
}

void SurvivalRockLarge::SetupEditorCallbacks()
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

void SurvivalRockLarge::CleanupEditorCallbacks()
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