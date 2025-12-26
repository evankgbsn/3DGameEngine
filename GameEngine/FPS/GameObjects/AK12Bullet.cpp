#include "AK12Bullet.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "../GameObjects/FPSPlayer.h"
#include "GameEngine/GameObject/Component/OrientedBoundingBoxComponent.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"

AK12Bullet::AK12Bullet() : 
    GameObject("AK12Bullet"),
    graphics(nullptr),
	speed(100.0f)
{
    RegisterGameObjectClassType<AK12Bullet>(this);
    RegisterNetworkObjectClassType<AK12Bullet>(this);
}

AK12Bullet::~AK12Bullet()
{
}

void AK12Bullet::Initialize()
{
    graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("AK12Bullet"), TextureManager::GetTexture("AK12Bullet"), TextureManager::GetTexture("AK12Bullet"));

    AddComponent(graphics, "Graphics");

	collider = new OrientedBoundingBoxComponent(graphics->GetModel()->GetVertices(), graphics->GetRotation());

	AddComponent(collider, "Collider");
	glm::mat4 transform(1.0f);

	Scene* main = SceneManager::GetRegisteredScene("Test");

	if (main != nullptr)
	{
		GameObject* localPlayer = nullptr;

		const std::unordered_map<std::string, GameObject*>& gameObjects = main->GetGameObjects();

		for (const auto& obj : gameObjects)
		{
			FPSPlayer* player = dynamic_cast<FPSPlayer*>(obj.second);

			if (player != nullptr)
			{
				if (GetSpawnerIP() == player->GetSpawnerIP())
				{
					transform = player->GetWeaponTransform();
				}
			}
		}
	}

	graphics->SetTransform(transform);
	direction = glm::normalize(transform[2]);

	body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, graphics->GetModel());
	body->AddForce(graphics->GetForward() * speed, RigidBodyComponent::ForceMode::IMPULSE);
	AddComponent(body, "RigidBody");

	onImpact = new std::function<void(GameObject*)>([this](GameObject* other)
		{
			Logger::Log("Contact: " + GetName() + " whth: " + other->GetName());
			if (NetworkManager::IsServer())
			{
				NetworkManager::Despawn(GetNetworkObjectID());
			}
		});

	body->RegisterContactCallback("Ground", onImpact);
	updateTime = TimeManager::SecondsSinceStart();
}

void AK12Bullet::Terminate()
{

	body->DeregisterContactCallback("Ground");
	delete onImpact;

	RemoveComponent("RigidBody");
	delete body;

    RemoveComponent("Graphics");
    delete graphics;

	RemoveComponent("Collider");
	delete collider;
}

void AK12Bullet::GameUpdate()
{
	body->SyncPhysics();

	body->Update();

	collider->UpdateCollider(graphics->GetTransform());
	
	if (TimeManager::SecondsSinceStart() - updateTime > 1.0f)
	{
		Logger::Log("Bullet Alive");
		updateTime = TimeManager::SecondsSinceStart();
	}
}

void AK12Bullet::EditorUpdate()
{
}

void AK12Bullet::Load()
{
    if (!ModelManager::ModelLoaded("AK12Bullet"))
    {
        ModelManager::LoadModel("AK12Bullet", "Assets/Model/AK12Bullet.gltf", false);
    }

	if (!TextureManager::TextureLoaded("AK12Bullet"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "AK12Bullet");
	}
}

void AK12Bullet::Unload()
{
}

glm::vec3 AK12Bullet::GetPosition() const
{
    return graphics->GetPosition();
}

glm::mat4 AK12Bullet::GetRotation() const
{
    return graphics->GetRotation();
}

glm::mat4 AK12Bullet::GetTransform() const
{
    return graphics->GetTransform();
}

void AK12Bullet::SetPosition(const glm::vec3& pos)
{
	graphics->SetPosition(pos);
}

void AK12Bullet::SetRotation(const glm::mat4& rot)
{
	graphics->SetRotation(rot);
}

bool AK12Bullet::Hovered() const
{
    return false;
}

void AK12Bullet::Deserialize()
{
}

void AK12Bullet::OnSpawn()
{
	NetworkObject::OnSpawn();

	Scene* scene = SceneManager::GetRegisteredScene("Test");

	if (scene != nullptr)
	{
		scene->RegisterGameObject(this, "AK12Bullet:" + std::to_string(GetNetworkObjectID()));
	}

	spawnTime = TimeManager::SecondsSinceStart();
}

void AK12Bullet::OnDespawn()
{
	NetworkObject::OnDespawn();
}

void AK12Bullet::OnDataReceived(const std::string& data)
{
	NetworkObject::OnDataReceived(data);

	std::string updateType;

	unsigned int i = 0;
	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			i++;
			break;
		}
		updateType += data[i];
	}

	std::string packetID;

	for (i; i < data.size(); i++)
	{
		if (data[i] == ' ')
		{
			i++;
			break;
		}
		packetID += data[i];
	}

	std::string updateData;

	for (const auto& character : std::string(data.begin() + updateType.size() + packetID.size() + 2, data.end()))
	{
		updateData += character;
	}

	if (!NetworkManager::IsServer())
	{
		if (updateType == "Position")
		{
			if (std::stoi(packetID) >= lastPositionPacket)
			{
				positionToSet = NetworkManager::ConvertDataToVec3(updateData);
			}

			lastPositionPacket = std::stoi(packetID);
		}
	}
}

void AK12Bullet::OnServerSpawnConfirmation(const std::string& IP)
{
	NetworkObject::OnServerSpawnConfirmation(IP);
}

void AK12Bullet::OnClientSpawnConfirmation()
{
	NetworkObject::OnClientSpawnConfirmation();
}
