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
#include "GameEngine/Renderer/Window/WindowManager.h"

PlayerShip::PlayerShip() :
	GameObject("PlayerShip"),
	graphics(nullptr),
	cam(nullptr),
	body(nullptr),
	move(nullptr),
	look(nullptr),
	speed(100000.0f),
	rotationSpeed(1.0f),
	positionUpdateInterval(0.05f),
	rotationUpdateInterval(0.05f),
	camOffset({0.0f, 5.5f, -8.0f}),
	positionToSet({0.0f, 0.0f, 0.0f}),
	rotationToSet(glm::mat4(1.0f)),
	movementForce({0.0f, 0.0f, 0.0f}),
	torque({ 0.0f, 0.0f, 0.0f }),
	gamepadTorqueX({ 0.0f, 0.0f, 0.0f }),
	gamepadTorqueY({ 0.0f, 0.0f, 0.0f })
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
		body->DisableGravity();
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
	if (IsClient())
	{
		SetPosition(positionToSet);
		SetRotation(rotationToSet);
	}
	
	UpdatePhysics();
	Look();
	Move();
	OrientCamera();
	SendServerPositionUpdates();
	SendServerRotationUpdates();
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
	static std::function<void(int)> escapeCursor = [](int keyCode)
		{
			WindowManager::GetWindow("Engine")->ToggleFullScreen();

			static bool enabled = false;

			if (!enabled)
			{
				InputManager::EnableCursor("Engine");
				enabled = true;
			}
			else
			{
				InputManager::DisableCursor("Engine");
				enabled = false;
			}
		};

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_ESCAPE, &escapeCursor, "FreeCursor");

	move = new std::function<void(int)>([this](int keyCode)
		{
			if (InputManager::IsGamepadConnected())
			{
				return;
			}
			ClientSend("Move " + std::to_string(movePacketNumber++) + " " + std::to_string(keyCode), false);
		});

	stopMove = new std::function<void(int)>([this](int keyCode)
		{
			if (InputManager::IsGamepadConnected())
			{
				return;
			}
			ClientSend("StopMove " + std::to_string(stopMovePacketNumber++) + " ", false);
		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, move, "Move");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, move, "Move");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, move, "Move");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, move, "Move");

	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_W, stopMove, "StopMove");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_A, stopMove, "StopMove");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_S, stopMove, "StopMove");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_D, stopMove, "StopMove");

	look = new std::function<void(const glm::vec2&)>([this](const glm::vec2& pos)
		{
			if (InputManager::IsGamepadConnected())
			{
				return;
			}

			static glm::vec2 lastPos = pos;

			glm::vec2 dif = pos - lastPos;

			ClientSend("Look " + std::to_string(lookPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(glm::vec3(dif, 0.0f)), false);

			lastPos = pos;
		});

	InputManager::DisableCursor("Engine");

	gamepadMoveX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (!InputManager::IsGamepadConnected())
			{
				return;
			}

			if (abs(value) > 0.5f)
			{
				ClientSend("Move " + std::to_string(movePacketNumber++) + " " + std::to_string((value < 0.0f) ? KEY_A : KEY_D), false);
			}
			else
			{
				ClientSend("StopMove " + std::to_string(stopMovePacketNumber++) + " ", false);
			}
		});

	gamepadMoveY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (!InputManager::IsGamepadConnected())
			{
				return;
			}

			if (abs(value) > 0.5f)
			{
				ClientSend("Move " + std::to_string(movePacketNumber++) + " " + std::to_string((value < 0.0f) ? KEY_W : KEY_S), false);
			}
			else
			{
				ClientSend("StopMove " + std::to_string(stopMovePacketNumber++) + " ", false);
			}
		});

	gamepadLookX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (!InputManager::IsGamepadConnected())
			{
				return;
			}

			if (abs(value) > 0.05f)
			{
				glm::vec3 dif = glm::vec3(10000.0f * value, 0.0, 0.0f);
				ClientSend("GamepadLookX " + std::to_string(lookPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(dif), false);
			}
			else
			{
				ClientSend("GamepadLookX " + std::to_string(lookPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(glm::vec3(0.0f)), false);
			}
		});

	gamepadLookY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (!InputManager::IsGamepadConnected())
			{
				return;
			}

			if (abs(value) > 0.05f)
			{
				glm::vec3 dif = glm::vec3(0.0f, 10000.0f * value, 0.0f);
				ClientSend("GamepadLookY " + std::to_string(lookPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(dif), false);
			}
			else
			{
				ClientSend("GamepadLookY " + std::to_string(lookPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(glm::vec3(0.0f)), false);
			}
		});

	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_X, gamepadMoveX, "MoveX");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_Y, gamepadMoveY, "MoveY");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_X, gamepadLookX, "LookX");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_Y, gamepadLookY, "LookY");
}

void PlayerShip::DeregisterInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "Move");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "Move");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "Move");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "Move");

	delete move;

	InputManager::DeregisterCallbackForKeyState(KEY_RELEASE, KEY_W, "StopMove");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASE, KEY_A, "StopMove");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASE, KEY_S, "StopMove");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASE, KEY_D, "StopMove");

	delete stopMove;

	delete look;

}

void PlayerShip::AddClientDataReceivedCallbacks()
{
	AddClientDataReceivedCallback("Position", clientDataReceivedCallbacks["Position"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			positionToSet = NetworkManager::ConvertDataToVec3(data);
		}));

	AddClientDataReceivedCallback("Rotation", clientDataReceivedCallbacks["Rotation"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			rotationToSet = NetworkManager::ConvertDataToMat4(data);
		}));
}

void PlayerShip::AddServerDataReceivedCallbacks()
{
	AddServerDataReceivedCallback("Move", serverDataReceivedCallbacks["Move"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			glm::vec3 disp(0.0f);

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

			movementForce = disp * speed;
		}));

	AddServerDataReceivedCallback("StopMove", serverDataReceivedCallbacks["StopMove"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			movementForce = glm::vec3(0.0f);
		}));

	AddServerDataReceivedCallback("Look", serverDataReceivedCallbacks["Look"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			glm::vec2 dif = NetworkManager::ConvertDataToVec3(data);
			torque = (graphics->GetForward() * rotationSpeed * dif.x) + (graphics->GetRight() * rotationSpeed * dif.y);
			lastLookPacketTime = TimeManager::SecondsSinceStart();
		}));

	AddServerDataReceivedCallback("GamepadLookX", serverDataReceivedCallbacks["GamepadLookX"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			glm::vec2 dif = NetworkManager::ConvertDataToVec3(data);
			gamepadTorqueX = (graphics->GetForward() * rotationSpeed * dif.x) + (graphics->GetRight() * rotationSpeed * dif.y);
			lastLookPacketTime = TimeManager::SecondsSinceStart();
		}));

	AddServerDataReceivedCallback("GamepadLookY", serverDataReceivedCallbacks["GamepadLookY"] = new std::function<void(const std::string&)>([this](const std::string& data)
		{
			glm::vec2 dif = NetworkManager::ConvertDataToVec3(data);
			gamepadTorqueY = (graphics->GetForward() * rotationSpeed * dif.x) + (graphics->GetRight() * rotationSpeed * dif.y);
			lastLookPacketTime = TimeManager::SecondsSinceStart();
		}));
}

void PlayerShip::SendServerPositionUpdates()
{
	if (IsServer())
	{
		static float lastSendTime = TimeManager::SecondsSinceStart();

		if (TimeManager::SecondsSinceStart() - lastSendTime > positionUpdateInterval)
		{
			glm::vec3 positionToSend = graphics->GetPosition();
			ServerSendAll("Position " + std::to_string(positionPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(positionToSend), {}, false);

			lastSendTime = TimeManager::SecondsSinceStart();
		}
	}
}

void PlayerShip::SendServerRotationUpdates()
{
	if (IsServer())
	{
		static float lastSendTime = TimeManager::SecondsSinceStart();

		if (TimeManager::SecondsSinceStart() - lastSendTime > rotationUpdateInterval)
		{
			ServerSendAll("Rotation " + std::to_string(rotationPacketNumber++) + " " + NetworkManager::ConvertMat4ToData(graphics->GetRotation()), {}, false);

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

void PlayerShip::Move()
{
	if (IsServer())
	{
		body->SetLinearVelocity(movementForce * TimeManager::DeltaTime());
		body->SetAngularVelocity((torque + gamepadTorqueX + gamepadTorqueY) * TimeManager::DeltaTime());

		if (TimeManager::SecondsSinceStart() - lastLookPacketTime > 0.1f)
		{
			torque = glm::vec3(0.0f);
			gamepadTorqueX = glm::vec3(0.0f);
			gamepadTorqueY = glm::vec3(0.0f);
		}
	}
}

void PlayerShip::Look()
{
	if (IsLocalClient())
	{
		InputManager::WhenCursorMoved(*look);
	}
}
