#include "SurvivalRockLarge.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/OrientedBoundingBoxComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Editor/Editor.h"

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
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("RockLarge"), TextureManager::GetTexture("Rock"), TextureManager::GetTexture("Rock"));
	graphics->SetShine(32.0f);
	collider = new OrientedBoundingBoxComponent(ModelManager::GetModel("RockLarge")->GetVertices());
	collider->UpdateCollider(graphics->GetTransform());
	SetupEditorCallbacks();

	AddComponent(graphics, "Graphics");
	AddComponent(collider, "Collider");
}

void SurvivalRockLarge::Terminate()
{
	CleanupEditorCallbacks();
	RemoveComponent("Graphics");
	RemoveComponent("Collider");
	delete graphics;
	delete collider;
}

void SurvivalRockLarge::GameUpdate()
{
	collider->UpdateCollider(graphics->GetTransform());
}

void SurvivalRockLarge::EditorUpdate()
{
	collider->UpdateCollider(graphics->GetTransform());
}

void SurvivalRockLarge::Load()
{
	if (!ModelManager::ModelLoaded("RockLarge"))
	{
		ModelManager::LoadModel("RockLarge", "Assets/Model/RockLarge.gltf", false);
	}

	if (!TextureManager::TextureLoaded("Rock"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "Rock");
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

	graphics = static_cast<GraphicsObjectTexturedLit*>(GetComponent("Graphics"));
	collider = static_cast<OrientedBoundingBoxComponent*>(GetComponent("Collider"));

	collider->UpdateCollider(graphics->GetTransform());

	CleanupEditorCallbacks();
	SetupEditorCallbacks();
}

bool SurvivalRockLarge::Hovered() const
{
	LineSegment3D line = Camera::CastLineFromCursorWithActiveCamera();

	if (collider != nullptr)
	{
		return collider->LineIntersect(line);
	}

	return false;
}

void SurvivalRockLarge::SetPosition(const glm::vec3& newPos)
{
	graphics->SetPosition(newPos);
	collider->UpdateCollider(graphics->GetTransform());
}

glm::vec3 SurvivalRockLarge::GetPosition() const
{
	return graphics->GetPosition();
}

void SurvivalRockLarge::SetRotation(const glm::mat4& newRot)
{
	graphics->SetRotation(newRot);
	collider->UpdateCollider(graphics->GetTransform());
}

glm::mat4 SurvivalRockLarge::GetRotation() const
{
	return graphics->GetRotation();
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
