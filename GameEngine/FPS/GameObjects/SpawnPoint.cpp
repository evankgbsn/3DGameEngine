#include "SpawnPoint.h"

#include "GameEngine/GameObject/Component/BoundingSphereComponent.h"
#include "GameEngine/Renderer/Camera/Camera.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Editor/Editor.h"
#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/Renderer/Model/ModelManager.h"

#include <glm/gtc/matrix_transform.hpp>

SpawnPoint::SpawnPoint() :
	GameObject("SpawnPoint")
{
	RegisterGameObjectClassType<SpawnPoint>(this);
}

SpawnPoint::~SpawnPoint()
{
}

void SpawnPoint::Initialize()
{
	sphere = new BoundingSphereComponent(1.0f);
	sphere->SetColor({ 0.0f, 1.0f, 1.0f });

	AddComponent(sphere, "BoundingSphere");

	arrow = new GraphicsObjectColored(ModelManager::GetModel("Arrow"), { 0.0f, 1.0f, 1.0f, 1.0f });
	arrow->SetRenderMode(GraphicsObject3DComponent::Mode::LINE);

	AddComponent(arrow, "ArrowGraphics");

	if (Engine::IsEditorBuild())
	{
		onEditorEnable = new std::function<void()>([this]()
			{
				if (!sphere->IsVisible())
				{
					sphere->ToggleVisibility();
				}
			});

		onEditorDisable = new std::function<void()>([this]()
			{
				if (sphere->IsVisible())
				{
					sphere->ToggleVisibility();
				}
			});

		Editor::RegisterOnEditorEnable(onEditorEnable);
		Editor::RegisterOnEditorDisable(onEditorDisable);
	}
}

void SpawnPoint::Terminate()
{
	if (Engine::IsEditorBuild())
	{
		Editor::DeregisterOnEditorEnable(onEditorEnable);
		Editor::DeregisterOnEditorDisable(onEditorDisable);
	}

	RemoveComponent("BoundingSphere");

	delete sphere;
}

void SpawnPoint::GameUpdate()
{
}

void SpawnPoint::EditorUpdate()
{
}

void SpawnPoint::Load()
{
	if (!ModelManager::ModelLoaded("Arrow"))
	{
		ModelManager::LoadModel("Arrow", "Assets/Model/Arrow.gltf", false);
	}
}

void SpawnPoint::Unload()
{
}

void SpawnPoint::Start()
{
	if (sphere->IsVisible())
	{
		sphere->ToggleVisibility();
	}
}

void SpawnPoint::End()
{
}

glm::vec3 SpawnPoint::GetPosition() const
{
	return sphere->GetOrigin();
}

glm::mat4 SpawnPoint::GetRotation() const
{
	return arrow->GetRotation();
}

glm::mat4 SpawnPoint::GetTransform() const
{
	return glm::translate(glm::mat4(1.0f),GetPosition());
}

void SpawnPoint::SetPosition(const glm::vec3& newPos)
{
	sphere->SetOrigin(newPos);
	sphere->UpdateTransform(GetTransform());
	arrow->SetPosition(newPos);
}

void SpawnPoint::SetRotation(const glm::mat4& newRot)
{
	arrow->SetRotation(newRot);
}

bool SpawnPoint::Hovered() const
{
	return sphere->LineSegmentIntersect(Camera::CastLineFromCursorWithActiveCamera());
}

void SpawnPoint::Deserialize()
{
	GameObject::Deserialize();
}
