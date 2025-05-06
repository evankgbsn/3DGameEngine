#include "RigidBox.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

RigidBox::RigidBox() :
    GameObject("RigidBox")
{
}

RigidBox::~RigidBox()
{
}

const std::vector<char> RigidBox::Serialize() const
{
    return std::vector<char>();
}

void RigidBox::Deserialize(const std::vector<char>& data)
{
}

void RigidBox::Initialize()
{
    graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("Cube"), TextureManager::GetTexture("Container"), TextureManager::GetTexture("ContainerSpec"));
    graphics->SetShine(0.5f);
    graphics->SetPosition({ 0.0f, 40.0f, 0.0f });
    graphics->Rotate(90.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    AddComponent(graphics, "Graphcis");

    rigidBody = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this);
    AddComponent(rigidBody, "RigidBody");


}

void RigidBox::Terminate()
{
}

void RigidBox::GameUpdate()
{
    rigidBody->SyncPhysics();
}

void RigidBox::EditorUpdate()
{
}

void RigidBox::Load()
{
    if (!ModelManager::ModelLoaded("Cube"))
    {
        ModelManager::LoadModel("Cube", "Assets/Model/Cube.gltf");
    }

    if (!TextureManager::TextureLoaded("Container"))
    {
        TextureManager::LoadTexture("Assets/Texture/container2.png", "Container");
    }

    if (!TextureManager::TextureLoaded("ContainerSpec"))
    {
        TextureManager::LoadTexture("Assets/Texture/container2_specular.png", "ContainerSpec");
    }
}

void RigidBox::Unload()
{
}

void RigidBox::SetPosition(const glm::vec3& newPosition)
{
    graphics->SetPosition(newPosition);
}

glm::vec3 RigidBox::GetPosition() const
{
    return graphics->GetPosition();
}

void RigidBox::SetRotation(const glm::mat4& newRotation)
{
    graphics->SetRotation(newRotation);
}

glm::mat4 RigidBox::GetRotation() const
{
    return graphics->GetRotation();
}
