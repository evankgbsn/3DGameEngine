#include "AK12Bullet.h"

#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "../GameObjects/FPSPlayer.h"

AK12Bullet::AK12Bullet() : 
    GameObject("AK12Bullet"),
    graphics(nullptr),
	speed(10.0f)
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
}

void AK12Bullet::Terminate()
{
    RemoveComponent("Graphics");
    delete graphics;
}

void AK12Bullet::GameUpdate()
{
	if (!NetworkManager::IsServer())
	{
		graphics->SetPosition(positionToSet);
	}
	else
	{
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
					if (player->IsLocal())
					{
						transform = player->GetWeaponTransform();
					}
				}
			}
		}

		graphics->Translate(glm::normalize(transform[2]) * speed * TimeManager::DeltaTime());

		static float updateTime = 0.0f;
		updateTime += TimeManager::DeltaTime();

		if (updateTime >= 0.001f)
		{
			ServerSendAll("Position " + std::to_string(positionPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(graphics->GetPosition()), {}, false);
			updateTime = 0.0f;
		}
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
}

void AK12Bullet::Unload()
{
}

glm::vec3 AK12Bullet::GetPosition() const
{
    return glm::vec3();
}

glm::mat4 AK12Bullet::GetRotation() const
{
    return glm::mat4();
}

glm::mat4 AK12Bullet::GetTransform() const
{
    return glm::mat4();
}

void AK12Bullet::SetPosition(const glm::vec3&)
{
}

void AK12Bullet::SetRotation(const glm::mat4&)
{
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
			static unsigned int lastPacket = std::stoi(packetID);

			if (std::stoi(packetID) >= lastPacket)
			{
				positionToSet = NetworkManager::ConvertDataToVec3(updateData);
			}

			lastPacket = std::stoi(packetID);
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
