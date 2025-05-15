#include "DirectionalLightGameObject.h"

#include "GameEngine/GameObject/Component/DirectionalLightComponent.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Collision/SphereWithVisualization.h"
#include "GameEngine/Renderer/Camera/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

DirectionalLightGameObject::DirectionalLightGameObject() :
	GameObject("DirectionalLight"),
	light(nullptr)
{
	RegisterGameObjectClassType<DirectionalLightGameObject>(this);
}

DirectionalLightGameObject::~DirectionalLightGameObject()
{
}

void DirectionalLightGameObject::Initialize()
{
	light = new DirectionalLightComponent({ -0.5f, -0.5f, 0.5f}, { 1.0f, 1.0f, 1.0f });
	AddComponent(light, "DirectionalLight");
}

void DirectionalLightGameObject::Terminate()
{
	delete light;
}

void DirectionalLightGameObject::GameUpdate()
{
	glm::mat4 rot(1.0f);
	static float rotationAngle = 0.1f;

	rot = glm::rotate(rot, rotationAngle * TimeManager::DeltaTime(), glm::vec3(1.0f, 0.0f, 0.0f));

	//light->SetDirection(rot * light->GetDirection());
}

void DirectionalLightGameObject::EditorUpdate()
{
}

void DirectionalLightGameObject::Load()
{
}

void DirectionalLightGameObject::Unload()
{
}

bool DirectionalLightGameObject::Hovered() const
{
	return light->GetCollider()->LineSegmentIntersect(Camera::CastLineFromCursorWithActiveCamera());
}

void DirectionalLightGameObject::SetPosition(const glm::vec3& pos)
{
	SphereWithVisualization* const collider = light->GetCollider();

	if (collider != nullptr)
	{
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, pos);

		collider->Transform(transform);
		collider->Update(transform);
	}
}

glm::vec3 DirectionalLightGameObject::GetPosition() const
{
	return light->GetCollider()->GetOrigin();
}

void DirectionalLightGameObject::Serialize()
{


	GameObject::Serialize();
}

void DirectionalLightGameObject::Deserialize()
{

	GameObject::Deserialize();
}
