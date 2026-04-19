#include "PlayerShip.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"

PlayerShip::PlayerShip() :
	GameObject("PlayerShip"),
	graphics(nullptr),
	cam(nullptr),
	body(nullptr),
	move(nullptr),
	look(nullptr),
	speed(10.0f),
	positionUpdateInterval(0.05f),
	camOffset({0.0f, 5.5f, -8.0f})
{
	RegisterGameObjectClassType<PlayerShip>(this);
	RegisterNetworkObjectClassType<PlayerShip>(this);
}

PlayerShip::~PlayerShip()
{
}

void PlayerShip::Initialize()
{
	InitializeLocalPlayer();

	InitializeServer();

	InitializeRemotePlayer();
}

void PlayerShip::InitializeLocalPlayer()
{
	if (IsLocalClient())
	{
		graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("DefaultShip"), "DefaultShip", "DefaultShipSpec", "DefaultShipNorm");
		graphics->SetShine(4.0f);
		graphics->SetPosition({ 0.0f, 0.0f, 0.0f });
		AddComponent(graphics, "Graphics");

		cam = new CameraComponent("Player:" + std::to_string(GetNetworkObjectID()));
		cam->SetActive();
		AddComponent(cam, "Camera");

		RegisterInput();
		AddClientDataReceivedCallbacks();
	}
}

void PlayerShip::InitializeServer()
{
	if (IsServer())
	{
		graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("DefaultShip"), "DefaultShip", "DefaultShipSpec", "DefaultShipNorm");
		graphics->SetShine(4.0f);
		graphics->SetPosition({ 0.0f, 0.0f, 0.0f });
		AddComponent(graphics, "Graphics");

		cam = new CameraComponent("Player:" + std::to_string(GetNetworkObjectID()));
		AddComponent(cam, "Camera");

		body = new RigidBodyComponent(RigidBodyComponent::Type::DYNAMIC, this, graphics->GetModel());
		AddComponent(body, "RigidBody");

		AddServerDataReceivedCallbacks();
	}
}

void PlayerShip::InitializeRemotePlayer()
{
	if (IsClient() && !IsLocalClient())
	{
		graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("DefaultShip"), "DefaultShip", "DefaultShipSpec", "DefaultShipNorm");
		graphics->SetShine(4.0f);
		graphics->SetPosition({ 0.0f, 0.0f, 0.0f });
		AddComponent(graphics, "Graphics");

		cam = new CameraComponent("Player:" + std::to_string(GetNetworkObjectID()));
		AddComponent(cam, "Camera");

		AddClientDataReceivedCallbacks();
	}
}

void PlayerShip::Terminate()
{
	TerminateLocalPlayer();

	TerminateServer();

	TerminateRemotePlayer();
}

void PlayerShip::TerminateLocalPlayer()
{
	if (IsLocalClient())
	{
		RemoveComponent("Graphics");
		delete graphics;

		RemoveComponent("Camera");
		delete cam;

		DeregisterInput();
	}
}

void PlayerShip::TerminateServer()
{
	if (IsServer())
	{
		RemoveComponent("Graphics");
		delete graphics;

		RemoveComponent("Camera");
		delete cam;

		RemoveComponent("RigidBody");
		delete body;
	}
}

void PlayerShip::TerminateRemotePlayer()
{
	if (IsClient() && !IsLocalClient())
	{
		RemoveComponent("Graphics");
		delete graphics;

		RemoveComponent("Camera");
		delete cam;
	}
}

void PlayerShip::GameUpdate()
{
	OrientCamera();
	SendServerPositionUpdates();
	UpdatePhysics();
}

void PlayerShip::EditorUpdate()
{
}

void PlayerShip::Load()
{
	if (!ModelManager::ModelLoaded("DefaultShip"))
	{
		ModelManager::LoadModel("DefaultShip", "Assets/Model/DefaultShip.gltf", false);
	}

	if (!TextureManager::TextureLoaded("DefaultShip"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "DefaultShip");
	}

	if (!TextureManager::TextureLoaded("DefaultShipSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "DefaultShipSpec");
	}

	if (!TextureManager::TextureLoaded("DefaultShipNorm"))
	{
		TextureManager::LoadTexture("Assets/Texture/Normal.png", "DefaultShipNorm");
	}
}

void PlayerShip::Unload()
{
}

void PlayerShip::OnSpawn()
{
	NetworkObject::OnSpawn();

	Scene* scene = SceneManager::GetRegisteredScene("AsteroidField");

	if (scene != nullptr)
	{
		scene->RegisterGameObject(this, "Player:" + std::to_string(GetNetworkObjectID()));
	}

	if (NetworkManager::IsServer())
	{
		onClientDisconnect = new std::function<void(const std::string&)>([this](const std::string& ID)
			{
				if (GetSpawnerID() == ID)
				{
					NetworkManager::Despawn(GetNetworkObjectID());
				}
			});

		NetworkManager::RegisterOnClientDisconnectFunction("Player:" + std::to_string(GetNetworkObjectID()), onClientDisconnect);
	}
}

void PlayerShip::OnDespawn()
{
	NetworkManager::DeregisterOnClientDisconnectFunction("Player:" + std::to_string(GetNetworkObjectID()));

	NetworkObject::OnDespawn();
}

glm::vec3 PlayerShip::GetPosition() const
{
	return graphics->GetPosition();
}

void PlayerShip::SetPosition(const glm::vec3& pos)
{
	if (body != nullptr)
	{
		body->SetPosition(pos);
	}

	graphics->SetPosition(pos);
}

glm::mat4 PlayerShip::GetRotation() const
{
	return graphics->GetRotation();
}

void PlayerShip::SetRotation(const glm::mat4& rot)
{
	if (body != nullptr)
	{
		body->SetRotation(rot);
	}

	graphics->SetRotation(rot);
}

glm::mat4 PlayerShip::GetTransform() const
{
	return graphics->GetTransform();
}

void PlayerShip::OnDataReceived(const std::string& data)
{
	NetworkObject::OnDataReceived(data);
}

void PlayerShip::OnServerSpawnConfirmation(const std::string& ID)
{
	NetworkObject::OnServerSpawnConfirmation(ID);
}

void PlayerShip::OnClientSpawnConfirmation()
{
	NetworkObject::OnClientSpawnConfirmation();
}

void PlayerShip::OrientCamera()
{
	glm::vec3 camPosition = graphics->GetPosition() + (camOffset.x * graphics->GetRight()) + (camOffset.y * graphics->GetUp()) + (camOffset.z * graphics->GetForward());
	cam->SetPosition(camPosition);
	cam->SetUpVector(graphics->GetUp());
	cam->SetTarget(graphics->GetPosition());
}

void PlayerShip::RegisterInput()
{
	move = new std::function<void(int)>([this](int keyCode)
		{
			ClientSend("Move " + std::to_string(movePacketNumber++) + " " + std::to_string(keyCode), false);
		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, move, "Move");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, move, "Move");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, move, "Move");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, move, "Move");
}

void PlayerShip::DeregisterInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "Move");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "Move");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "Move");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "Move");

	delete move;
}

void PlayerShip::AddClientDataReceivedCallbacks()
{
	AddClientDataReceivedCallback("Position", clientDataReceivedCallbacks["Position"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			SetPosition(NetworkManager::ConvertDataToVec3(data));
		}));
}

void PlayerShip::AddServerDataReceivedCallbacks()
{
	AddServerDataReceivedCallback("Move", serverDataReceivedCallbacks["Move"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			glm::vec3 disp;

			switch (std::stoi(data))
			{
			case KEY_W:
				disp = graphics->GetForward();
				break;
			case KEY_A:
				disp = graphics->GetRight();
				break;
			case KEY_S:
				disp = -graphics->GetForward();
				break;
			case KEY_D:
				disp = -graphics->GetRight();
				break;
			default:
				break;
			}

			body->AddForce(disp * speed * TimeManager::DeltaTime(), RigidBodyComponent::ForceMode::IMPULSE);
		}));
}

void PlayerShip::SendServerPositionUpdates()
{
	if (IsServer())
	{
		static float lastSendTime = TimeManager::SecondsSinceStart();

		if (TimeManager::SecondsSinceStart() - lastSendTime > positionUpdateInterval)
		{
			ServerSendAll("Position " + std::to_string(positionPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(graphics->GetPosition()), {}, false);

			lastSendTime = TimeManager::SecondsSinceStart();
		}
	}
}

void PlayerShip::UpdatePhysics()
{
	if (IsServer())
	{
		body->Update();
		body->SyncPhysics();
	}
}
