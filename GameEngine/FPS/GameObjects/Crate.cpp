#include "Crate.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

Crate::Crate() :
    GameObject("Crate")
{
    RegisterGameObjectClassType<Crate>(this);
}

Crate::~Crate()
{
}

void Crate::Initialize()
{
    graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("Crate"), TextureManager::GetTexture("Crate"), TextureManager::GetTexture("CrateSpec"));
    graphics->SetShine(32.0f);
    graphics->SetPosition({ 0.0f, 3.0f, 0.0f });
   
    AddComponent(graphics, "Graphics");

    body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, ModelManager::GetModel("Crate"));
    body->SyncPhysics();

    AddComponent(body, "RigidBody");
}

void Crate::Terminate()
{
    RemoveComponent("Graphics");
    RemoveComponent("RigidBody");

    delete graphics;
    delete body;
}

void Crate::GameUpdate()
{
    body->SyncPhysics();
    body->Update();
}

void Crate::EditorUpdate()
{
}

void Crate::Load()
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

void Crate::Unload()
{
}

glm::vec3 Crate::GetPosition() const
{
    return graphics->GetPosition();
}

glm::mat4 Crate::GetRotation() const
{
    return graphics->GetRotation();
}

glm::mat4 Crate::GetTransform() const
{
    return graphics->GetTransform();
}

void Crate::SetPosition(const glm::vec3& newPos)
{
    graphics->SetPosition(newPos);
    body->SetPosition(newPos);

}

void Crate::SetRotation(const glm::mat4& newRot)
{
    graphics->SetRotation(newRot);
    body->SetRotation(newRot);
}

bool Crate::Hovered() const
{
    return body->Hovered();
}

void Crate::Deserialize()
{
    GameObject::Deserialize();
    body->SetOwner(this);
}
