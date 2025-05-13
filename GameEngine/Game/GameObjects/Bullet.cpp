#include "Bullet.h"

#include "GameEngine/GameObject/Component/GraphicsObjectColored.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"

Bullet::Bullet() :
    GameObject("Bullet"),
    graphics(nullptr),
    body(nullptr)
{
}

Bullet::~Bullet()
{
}

void Bullet::SetDirection(const glm::vec3& dir)
{
    direction = dir;
}

void Bullet::SetOrigin(const glm::vec3& x)
{
    origin = x;
}

void Bullet::SetPosition(const glm::vec3& newPos)
{
    graphics->SetPosition(newPos);
}

void Bullet::SetRotation(const glm::mat4& newRot)
{
    graphics->SetRotation(newRot);
}

glm::vec3 Bullet::GetPosition() const
{
    return graphics->GetPosition();
}

glm::mat4 Bullet::GetRotation() const
{
    return graphics->GetRotation();
}

glm::mat4 Bullet::GetTransform() const
{
    return graphics->GetTransform();
}

const std::vector<char> Bullet::Serialize() const
{
    return std::vector<char>();
}

void Bullet::Deserialize(const std::vector<char>& data)
{
}

void Bullet::Initialize()
{
  
}

void Bullet::Terminate()
{
}

void Bullet::GameUpdate()
{
    body->SyncPhysics();
}

void Bullet::EditorUpdate()
{
}

void Bullet::Load()
{
}

void Bullet::Unload()
{
}

void Bullet::Start()
{
    graphics = new GraphicsObjectColored(ModelManager::GetModel("Sphere"), { 1.0f, 1.0f, 1.0f, 1.0f });
    graphics->SetPosition(origin);
    AddComponent(graphics, "Graphics");

    body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, graphics->GetModel());
    AddComponent(body, "RigidBody");

    body->AddForce(direction * 50000.0f);
}
