#include "PlaneObj.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

PlaneObj::PlaneObj() :
    GameObject("PlaneObj")
{
    RegisterGameObjectClassType<PlaneObj>(this);
}

PlaneObj::~PlaneObj()
{

}

void PlaneObj::Initialize()
{
    graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("Plane"), TextureManager::GetTexture("Plane"), TextureManager::GetTexture("Plane"));
    graphics->SetShine(32.0f);

    body = new RigidBodyComponent(RigidBodyComponent::Type::STATIC, this, graphics->GetModel());

    body->SyncPhysics();
}

void PlaneObj::Terminate()
{
    delete body;

    delete graphics;
    
}

void PlaneObj::GameUpdate()
{
    body->SyncPhysics();
}

void PlaneObj::EditorUpdate()
{
}

void PlaneObj::Load()
{
    if (!ModelManager::ModelLoaded("Plane"))
    {
        ModelManager::LoadModel("Plane", "Assets/Model/Plane.gltf", false);
    }

    if (!TextureManager::TextureLoaded("Plane"))
    {
        TextureManager::LoadTexture("Assets/Texture/grey.png", "Plane");
    }
}

void PlaneObj::Unload()
{
}

glm::vec3 PlaneObj::GetPosition() const
{
    return graphics->GetPosition();
}

glm::mat4 PlaneObj::GetRotation() const
{
    return graphics->GetRotation();
}

glm::mat4 PlaneObj::GetTransform() const
{
    return graphics->GetTransform();
}

void PlaneObj::SetPosition(const glm::vec3& pos)
{
    graphics->SetPosition(pos);
}

void PlaneObj::SetRotation(const glm::mat4& rot)
{
    graphics->SetRotation(rot);
}

bool PlaneObj::Hovered() const
{
    return false;
}
