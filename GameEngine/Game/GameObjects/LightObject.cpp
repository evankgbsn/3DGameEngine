#include "LightObject.h"

#include "GameEngine/Renderer/Light/SpotLight.h"
#include "GameEngine/Renderer/Light/LightManager.h"
#include "GameEngine/Collision/SphereWithVisualization.h"
#include "GameEngine/Renderer/Camera/Camera.h"

LightObject::LightObject() :
	spotLight(nullptr)
{
}

LightObject::~LightObject()
{
}

void LightObject::Initialize()
{
	spotLight = LightManager::CreateSpotLight({ 5.0f, 5.0f, 5.0f, 0.0f }, { 5.0f, 5.0f, 5.0f }, { 0.0f, -1.0f, 0.3f });
}

void LightObject::Terminate()
{
	LightManager::Delete(spotLight);
}

void LightObject::GameUpdate()
{
}

void LightObject::EditorUpdate()
{
}

void LightObject::Load()
{
}

void LightObject::Unload()
{
}

bool LightObject::Hovered() const
{
	return spotLight->GetCollider()->LineSegmentIntersect(Camera::CastLineFromCursorWithActiveCamera());
}

void LightObject::SetPosition(const glm::vec3& pos)
{
	spotLight->SetPosition(pos);
}
