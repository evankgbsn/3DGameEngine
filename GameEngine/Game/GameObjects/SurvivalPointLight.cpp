#include "SurvivalPointLight.h"

#include "GameEngine/GameObject/Component/PointLightComponent.h"
#include "GameEngine/Editor/Editor.h"

SurvivalPointLight::SurvivalPointLight() :
    GameObject("SurvivalPointLight"),
    light(nullptr)
{
}

SurvivalPointLight::~SurvivalPointLight()
{
}

void SurvivalPointLight::Initialize()
{
    light = new PointLightComponent();

    light->SetColor({ 10.0f, 10.0f, 10.0f });
    light->SetPosition({ 5.0f, 5.0f, 5.0f });

    AddComponent(light, "PointLight");

    SetupEditorCallbacks();
}

void SurvivalPointLight::Terminate()
{
    CleanupEditorCallbacks();

    if (light != nullptr)
    {
        delete light;
    }
}

void SurvivalPointLight::GameUpdate()
{
}

void SurvivalPointLight::EditorUpdate()
{
}

void SurvivalPointLight::Load()
{
}

void SurvivalPointLight::Unload()
{
}

void SurvivalPointLight::Start()
{
    (*onEditorDisable)();
}

void SurvivalPointLight::Serialize()
{
    GameObject::Serialize();
}

void SurvivalPointLight::Deserialize()
{
    GameObject::Deserialize();

    light = dynamic_cast<PointLightComponent*>(GetComponent("PointLight"));
}

bool SurvivalPointLight::Hovered() const
{
    if (light != nullptr)
    {
        return light->Hovered();
    }

    return false;
}

void SurvivalPointLight::SetPosition(const glm::vec3& newPos)
{
    if (light != nullptr)
    {
        light->SetPosition(newPos);
    }
}

glm::vec3 SurvivalPointLight::GetPosition() const
{
    if (light != nullptr)
    {
        return light->GetPosition();
    }

    return glm::vec3();
}

void SurvivalPointLight::SetRotation(const glm::mat4& newRot)
{
}

glm::mat4 SurvivalPointLight::GetRotation() const
{
    return glm::mat4();
}

void SurvivalPointLight::SetupEditorCallbacks()
{
    onEditorEnable = new std::function<void()>([this]()
        {
            if (light != nullptr)
            {
                light->SetColliderVisibility(true);
            }
        });

    onEditorDisable = new std::function<void()>([this]()
        {
            if (light != nullptr)
            {
                light->SetColliderVisibility(false);
            }
        });

    Editor::RegisterOnEditorEnable(onEditorEnable);
    Editor::RegisterOnEditorDisable(onEditorDisable);
}

void SurvivalPointLight::CleanupEditorCallbacks()
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