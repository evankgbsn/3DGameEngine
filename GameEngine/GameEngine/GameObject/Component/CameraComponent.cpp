#include "CameraComponent.h"

#include "../../Renderer/Camera/CameraManager.h"
#include "../../Renderer/Window/WindowManager.h"
#include "../../Engine.h"

CameraComponent::CameraComponent()
{
	RegisterComponentClassType<CameraComponent>(this);
}

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

const glm::vec3& CameraComponent::GetCursorWorldPosition() const
{
	Camera& cam = CameraManager::GetCamera(name);

	// Screen space to world space for object picking.
	Window* window = WindowManager::GetWindow("Engine");
	glm::vec2 cursorPos = window->GetCursorPosition();
	glm::mat4 invPersp = glm::inverse(cam.GetProjection());
	glm::mat4 invView = glm::inverse(cam.GetView());
	glm::mat4 screenToNDC(
		glm::vec4((float)window->GetWidth() / 2.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0, -(float)window->GetHeight() / 2.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4((float)window->GetWidth() / 2.0f, (float)window->GetHeight() / 2.0f, 0.5f, 1.0f)
	);
	screenToNDC = glm::inverse(screenToNDC);

	glm::vec4 x = glm::vec4(cursorPos.x, cursorPos.y, 1.0f, 1.0f);
	x = screenToNDC * x;
	x = invPersp * x;
	x = invView * x;
	x /= x.w;

	return x;
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

void CameraComponent::Serialize()
{
	savedStrings["Name"] = name;
	savedVec3s["Position"] = GetPosition();
	savedVec3s["Target"] = GetTarget();
}

void CameraComponent::Deserialize()
{
	name = savedStrings["Name"];
	CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, name, Engine::GetWindow());

	Camera& cam = CameraManager::GetCamera(name);
	cam.SetPosition(savedVec3s["Position"]);
	cam.SetTarget(savedVec3s["Target"]);
}

void CameraComponent::Update()
{
	Camera& cam = CameraManager::GetCamera(name);
	cam.SetWindow(WindowManager::GetWindow("Engine"));
}
