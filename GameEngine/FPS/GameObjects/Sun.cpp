#include "Sun.h"

#include "GameEngine/GameObject/Component/DirectionalLightComponent.h"
#include "GameEngine/Renderer/Light/LightManager.h"

Sun::Sun() :
    GameObject("Sun")
{
    RegisterGameObjectClassType<Sun>(this);
}

Sun::~Sun()
{
}

void Sun::Initialize()
{
    light = new DirectionalLightComponent({ -0.3, -0.3, -0.3 }, { 0.5, 0.25, 0.25 });
    LightManager::SetAmbientIntensity(0.5f);
}

void Sun::Terminate()
{
    delete light;
}

void Sun::GameUpdate()
{
}

void Sun::EditorUpdate()
{
}

void Sun::Load()
{
}

void Sun::Unload()
{
}

glm::vec3 Sun::GetPosition() const
{
    return glm::vec3();
}

glm::mat4 Sun::GetRotation() const
{
    return glm::mat4();
}

glm::mat4 Sun::GetTransform() const
{
    return glm::mat4();
}

void Sun::SetPosition(const glm::vec3&)
{
}

void Sun::SetRotation(const glm::mat4&)
{
}

bool Sun::Hovered() const
{
    return false;
}
