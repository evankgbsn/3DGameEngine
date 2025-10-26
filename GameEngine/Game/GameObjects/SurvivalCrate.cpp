#include "SurvivalCrate.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/OrientedBoundingBoxComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Editor/Editor.h"

SurvivalCrate::SurvivalCrate() :
	GameObject("SurvivalCrate")
{
	RegisterGameObjectClassType<SurvivalCrate>(this);
}

SurvivalCrate::~SurvivalCrate()
{
}

void SurvivalCrate::Initialize()
{
	graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("Crate"), TextureManager::GetTexture("Crate"), TextureManager::GetTexture("CrateSpec"));
	graphics->SetShine(32.0f);
	collider = new OrientedBoundingBoxComponent(ModelManager::GetModel("Crate")->GetVertices());
	collider->UpdateCollider(graphics->GetTransform());
	SetupEditorCallbacks();

	AddComponent(graphics, "Graphics");
	AddComponent(collider, "Collider");
}

void SurvivalCrate::Terminate()
{
	CleanupEditorCallbacks();
	delete graphics;
}

void SurvivalCrate::GameUpdate()
{
	collider->UpdateCollider(graphics->GetTransform());
}

void SurvivalCrate::EditorUpdate()
{
	collider->UpdateCollider(graphics->GetTransform());
}

void SurvivalCrate::Load()
{
	if (!ModelManager::ModelLoaded("Crate"))
	{
		ModelManager::LoadModel("Crate", "Assets/Model/Cube.gltf", false);
	}

	if (!TextureManager::TextureLoaded("Crate"))
	{
		TextureManager::LoadTexture("Assets/Texture/container2.png", "Crate");
	}

	if (!TextureManager::TextureLoaded("CrateSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/container2_specular.png", "CrateSpec");
	}
}

void SurvivalCrate::Unload()
{
}

void SurvivalCrate::Start()
{
	(*onEditorDisable)();
}

void SurvivalCrate::Serialize()
{
	GameObject::Serialize();
}

void SurvivalCrate::Deserialize()
{
	GameObject::Deserialize();

	graphics = static_cast<GraphicsObjectTexturedLit*>(GetComponent("Graphics"));
	collider = static_cast<OrientedBoundingBoxComponent*>(GetComponent("Collider"));

	collider->UpdateCollider(graphics->GetTransform());

	CleanupEditorCallbacks();
	SetupEditorCallbacks();
}

bool SurvivalCrate::Hovered() const
{
	LineSegment3D line = Camera::CastLineFromCursorWithActiveCamera();

	if (collider != nullptr)
	{
		return collider->LineIntersect(line);
	}

	return false;
}

void SurvivalCrate::SetPosition(const glm::vec3& newPos)
{
	graphics->SetPosition(newPos);
	collider->UpdateCollider(graphics->GetTransform());
}

glm::vec3 SurvivalCrate::GetPosition() const
{
	return graphics->GetPosition();
}

void SurvivalCrate::SetRotation(const glm::mat4& newRot)
{
	graphics->SetRotation(newRot);
	collider->UpdateCollider(graphics->GetTransform());
}

glm::mat4 SurvivalCrate::GetRotation() const
{
	return graphics->GetRotation();
}

void SurvivalCrate::SetupEditorCallbacks()
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

void SurvivalCrate::CleanupEditorCallbacks()
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
