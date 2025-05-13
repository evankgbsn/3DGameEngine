#include "CameraComponent.h"

#include "../../Renderer/Camera/CameraManager.h"
#include "../../Engine.h"

CameraComponent::CameraComponent(const std::string& n) :
	name(n)
{
	RegisterComponentClassType<CameraComponent>(this);
	CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, name, Engine::GetWindow());
}

CameraComponent::~CameraComponent()
{
	CameraManager::DestroyCamera(name);
}

void CameraComponent::SetActive() const
{
	CameraManager::SetActiveCamera(name);
}

void CameraComponent::Rotate(const glm::vec3& axis, const float& angle)
{
	CameraManager::GetCamera(name).Rotate(axis, angle);
}

void CameraComponent::Translate(const glm::vec3& translation)
{
	CameraManager::GetCamera(name).Translate(translation);
}

glm::vec3 CameraComponent::GetForwardVector() const
{
	return CameraManager::GetCamera(name).GetForwardVector();
}

glm::vec3 CameraComponent::GetRightVector() const
{
	return CameraManager::GetCamera(name).GetRightVector();
}

glm::vec3 CameraComponent::GetUpVector() const
{
	return CameraManager::GetCamera(name).GetUpVector();
}

const float& CameraComponent::GetFOV() const
{
	return CameraManager::GetCamera(name).GetFOV();
}

const float& CameraComponent::GetNear() const
{
	return CameraManager::GetCamera(name).GetNear();
}

const float& CameraComponent::GetFar() const
{
	return CameraManager::GetCamera(name).GetFar();
}

const glm::vec3& CameraComponent::GetPosition() const
{
	return CameraManager::GetCamera(name).GetPosition();
}

const glm::mat4& CameraComponent::GetView() const
{
	return CameraManager::GetCamera(name).GetView();
}

const glm::vec3& CameraComponent::GetTarget() const
{
	return CameraManager::GetCamera(name).GetTarget();
}

const glm::mat4& CameraComponent::GetProjection() const
{
	return CameraManager::GetCamera(name).GetProjection();
}

void CameraComponent::SetPosition(const glm::vec3& position)
{
	CameraManager::GetCamera(name).SetPosition(position);
}

void CameraComponent::SetTarget(const glm::vec3& newTarget)
{
	CameraManager::GetCamera(name).SetTarget(newTarget);
}

void CameraComponent::SetView(const glm::mat4& newView)
{
	CameraManager::GetCamera(name).SetView(newView);
}

void CameraComponent::SetProjection(const glm::mat4& newProjection)
{
	CameraManager::GetCamera(name).SetProjection(newProjection);
}

void CameraComponent::SetFOV(const float& newFOV)
{
	CameraManager::GetCamera(name).SetFOV(newFOV);
}

void CameraComponent::SetAspectRatio(const float& newAspectRatio)
{
	CameraManager::GetCamera(name).SetAspectRatio(newAspectRatio);
}

void CameraComponent::SetNear(const float& newNear)
{
	CameraManager::GetCamera(name).SetNear(newNear);
}

void CameraComponent::SetFar(const float& newFar)
{
	CameraManager::GetCamera(name).SetFar(newFar);
}

const std::vector<char> CameraComponent::Serialize() const
{
	return std::vector<char>();
}

void CameraComponent::Deserialize(const std::vector<char>& data)
{
}

void CameraComponent::Update()
{
}
