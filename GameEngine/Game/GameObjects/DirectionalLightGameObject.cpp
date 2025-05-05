#include "DirectionalLightGameObject.h"

#include "GameEngine/Renderer/Light/LightManager.h"
#include "GameEngine/Renderer/Light/DirectionalLight.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Collision/SphereWithVisualization.h"
#include "GameEngine/Renderer/Camera/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

DirectionalLightGameObject::DirectionalLightGameObject() :
	GameObject("DirectionalLight"),
	light(nullptr)
{
}

DirectionalLightGameObject::~DirectionalLightGameObject()
{
}

void DirectionalLightGameObject::Initialize()
{
	light = LightManager::CreateDirectionalLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { -0.5f, -0.5f, 0.5f, 0.0f });
	LightManager::SetAmbientIntensity(0.02f);
}

void DirectionalLightGameObject::Terminate()
{
	LightManager::Delete(light);
}

void DirectionalLightGameObject::GameUpdate()
{
	glm::mat4 rot(1.0f);
	static float rotationAngle = 0.1f;

	rot = glm::rotate(rot, rotationAngle * TimeManager::DeltaTime(), glm::vec3(1.0f, 0.0f, 0.0f));

	light->SetDirection(rot * light->GetDirection());
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

const std::vector<char> DirectionalLightGameObject::Serialize() const
{
	return std::vector<char>();
}

void DirectionalLightGameObject::Deserialize(const std::vector<char>& data)
{
}
