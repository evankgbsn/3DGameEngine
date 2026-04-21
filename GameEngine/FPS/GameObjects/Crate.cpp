#include "Crate.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"

Crate::Crate() :
    GameObject("Crate"),
    rotationToSet(glm::mat4(1.0f)),
    positionToSet(glm::vec3(1.0f)),
    positionPacketNumber(0ULL),
    rotationPacketNumber(0ULL),
    updateTime(0.0f),
    updateInterval(0.001f),
    body(nullptr),
    graphics(nullptr)
{
    RegisterGameObjectClassType<Crate>(this);
    RegisterNetworkObjectClassType<Crate>(this);
    MakeNetTypeServer();
}

Crate::~Crate()
{
}

void Crate::Hit(const glm::vec3& position, const glm::vec3& direction, float strength)
{
    body->AddForceAtPosition(direction * strength, position);
}

void Crate::Initialize()
{
    graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("Crate"), "Crate", "CrateSpec", "CrateNormal");
    graphics->SetShine(32.0f);
    graphics->SetPosition({ 0.0f, 3.0f, 0.0f });
   
    AddComponent(graphics, "Graphics");

    body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, ModelManager::GetModel("Crate"));
    body->SyncPhysics();

    AddComponent(body, "RigidBody");

    AddClientDataReceivedCallback("Position", positionReceivedCallback = new std::function<void(const std::string&)>([this](const std::string& data)
        {
            positionToSet = NetworkManager::ConvertDataToVec3(data);
        }));

    AddClientDataReceivedCallback("Rotation", rotationReceivedCallback = new std::function<void(const std::string&)>([this](const std::string& data)
        {
            rotationToSet = NetworkManager::ConvertDataToMat4(data);
        }));

}

void Crate::Terminate()
{
    RemoveClientDataReceivedCallback("Position");
    RemoveClientDataReceivedCallback("Rotation");

    delete positionReceivedCallback;
    delete rotationReceivedCallback;

    RemoveComponent("Graphics");
    RemoveComponent("RigidBody");

    delete graphics;
    delete body;
}

void Crate::GameUpdate()
{
    if (IsServer())
    {
        body->SyncPhysics();
        body->Update();

        updateTime += TimeManager::DeltaTime();

        if (updateTime >= updateInterval)
        {
            ServerSendAll(FormatPacket("Position", positionPacketNumber++, NetworkManager::ConvertVec3ToData(body->GetPosition())), {}, false);
            ServerSendAll(FormatPacket("Rotation", rotationPacketNumber++, NetworkManager::ConvertMat4ToData(body->GetRotation())), {}, false);

            updateTime = 0.0f;
        }
    }
    else
    {
        SetPosition(positionToSet);
        SetRotation(rotationToSet);
    }
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

    if (!TextureManager::TextureLoaded("CrateNormal"))
    {
        TextureManager::LoadTexture("Assets/Texture/Normal.png", "CrateNormal");
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
    positionToSet = body->GetPosition();
    rotationToSet = body->GetRotation();
}

void Crate::OnServerSpawnConfirmation(const std::string& IP)
{
    NetworkObject::OnServerSpawnConfirmation(IP);
}

void Crate::OnClientSpawnConfirmation()
{
    NetworkObject::OnClientSpawnConfirmation();
}

void Crate::OnSpawn()
{
    NetworkObject::OnSpawn();

    Scene* scene = SceneManager::GetRegisteredScene("Test");

    if (scene != nullptr)
    {
        scene->RegisterGameObject(this, "Crate:" + std::to_string(GetNetworkObjectID()));
    }
}

void Crate::OnDespawn()
{
    NetworkObject::OnDespawn();
}

void Crate::OnDataReceived(const std::string& data)
{
    NetworkObject::OnDataReceived(data);
}
