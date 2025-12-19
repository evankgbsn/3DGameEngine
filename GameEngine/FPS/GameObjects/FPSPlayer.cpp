#include "FPSPlayer.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedAnimatedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/RigidBodyComponent.h"
#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/GameObject/Component/AnimatedColliderComponent.h"
#include "GameEngine/Renderer/Animation/Armature.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Editor/Editor.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/GameObject/Component/CharacterControllerComponent.h"
#include "GameEngine/Networking/NetworkManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"

FPSPlayer::FPSPlayer() :
	GameObject("FPSPlayer")
{
	RegisterGameObjectClassType<FPSPlayer>(this);
	RegisterNetworkObjectClassType<FPSPlayer>(this);
}

FPSPlayer::~FPSPlayer()
{
}

void FPSPlayer::Initialize()
{
	characterGraphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::GetModel("Character"), TextureManager::GetTexture("Character"), TextureManager::GetTexture("Character"));
	characterGraphics->SetClip(0);
	characterGraphics->SetShine(32.0f);
	characterGraphics->SetPosition({ 0.0f, 20.0f, 0.0f });
	characterGraphics->SetSpeed(1.0f);

	AddComponent(characterGraphics, "Graphics");

	positionToSet = characterGraphics->GetPosition();

	hitBox = new AnimatedColliderComponent(characterGraphics);
	hitBox->Update();

	AddComponent(hitBox, "AnimatedCollider");
	
	if (NetworkManager::IsServer())
	{
		controller = new CharacterControllerComponent("FPSPlayer" + std::to_string(GetNetworkObjectID()), 0.35f, 1.0f, characterGraphics->GetPosition());
		AddComponent(controller, "Controller");
	}

	cam = new CameraComponent("FPSCharacter:" + std::to_string(GetNetworkObjectID()));

	cam->SetPosition(hitBox->GetJointTransform("Head")[3] + glm::normalize(hitBox->GetJointTransform("Head")[0]) * 0.5f);
	cam->SetTarget({ 0.0f, 0.0f, 30.0f });
	cam->SetFOV(20.0f);
	cam->SetNear(0.01f);

	AddComponent(cam, "Camera");

	if (SpawnedFromLocalSpawnRequest())
	{
		RegisterInput();
	}

	RegisterEditorToggleCallbacks();

	InputManager::DisableCursor("Engine");
}

void FPSPlayer::Terminate()
{
	DeregisterEditorToggleCallbacks();

	if (SpawnedFromLocalSpawnRequest())
	{
		DeregisterInput();
	}

	if (NetworkManager::IsServer())
	{
		RemoveComponent("Controller");
		delete controller;
		delete onClientDisconnect;
	}

	RemoveComponent("Camera");
	RemoveComponent("Graphics");
	RemoveComponent("AnimatedCollider");

	delete hitBox;
	delete characterGraphics;
	delete cam;
}

void FPSPlayer::GameUpdate()
{
	if (!NetworkManager::IsServer())
	{
		SetPosition(positionToSet);
		SetRotation(rotationToSet);
		cam->SetPosition(camPositionToSet);
	}

	hitBox->Update();

	glm::vec3 camRight = cam->GetRightVector();
	glm::vec3 newForward = glm::normalize(glm::cross(camRight, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 newUp = glm::normalize(glm::cross(newForward, camRight));

	characterGraphics->SetRotation(glm::mat4(glm::vec4(-camRight, 0.0f), glm::vec4(newUp, 0.0f), glm::vec4(-newForward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

	if (NetworkManager::IsServer())
	{
		cam->SetPosition(hitBox->GetJointTransform("Head")[3] + glm::normalize(hitBox->GetJointTransform("Head")[0]) * 0.5f);

		glm::vec3 camRight = cam->GetRightVector();
		glm::vec3 newForward = glm::normalize(glm::cross(camRight, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 newUp = glm::normalize(glm::cross(newForward, camRight));

		characterGraphics->SetRotation(glm::mat4(glm::vec4(-camRight, 0.0f), glm::vec4(newUp, 0.0f), glm::vec4(-newForward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

		controller->Update();
		characterGraphics->SetPosition(controller->GetPosition());

		static float updateTime = 0.0f;
		updateTime += TimeManager::DeltaTime();

		if (updateTime >= 0.001f)
		{
			ServerSendAll("Position " + std::to_string(positionPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(controller->GetPosition()), {}, false);
			ServerSendAll("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), {}, false);
			ServerSendAll("Rotation " + std::to_string(rotationPacketNumber++) + " " + NetworkManager::ConvertMat4ToData(characterGraphics->GetRotation()), {}, false);
			ServerSendAll("CameraPosition " + std::to_string(cameraPositionPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetPosition()), {}, false);
			updateTime = 0.0f;
		}

	}

	if (SpawnedFromLocalSpawnRequest())
	{
		InputManager::WhenCursorMoved(*whenCursorMove);
	}

}

void FPSPlayer::EditorUpdate()
{
	hitBox->Update();
}

void FPSPlayer::Load()
{
	if (!ModelManager::ModelLoaded("Character"))
	{
		ModelManager::LoadModel("Character", "Assets/Model/Woman.gltf", false);
	}

	if (!TextureManager::TextureLoaded("Character"))
	{
		TextureManager::LoadTexture("Assets/Texture/Woman.png", "Character");
	}

}

void FPSPlayer::Unload()
{
}

glm::vec3 FPSPlayer::GetPosition() const
{
	return characterGraphics->GetPosition();
}

glm::mat4 FPSPlayer::GetRotation() const
{
	return characterGraphics->GetRotation();
}

glm::mat4 FPSPlayer::GetTransform() const
{
	return characterGraphics->GetTransform();
}

void FPSPlayer::SetPosition(const glm::vec3& pos)
{
	characterGraphics->SetPosition(pos);
}

void FPSPlayer::SetRotation(const glm::mat4& rot)
{
	characterGraphics->SetRotation(rot);
}

bool FPSPlayer::Hovered() const
{
	return false;
}

void FPSPlayer::Start()
{
	if (SpawnedFromLocalSpawnRequest())
	{
		cam->SetActive();
	}

	if (hitBox->IsVisible())
	{
		hitBox->ToggleVisibility();
	}
}

void FPSPlayer::RegisterInput()
{
	whenCursorMove = new std::function<void(const glm::vec2&)>([this](const glm::vec2& cursorPos)
		{
			static glm::vec2 prevPos = cursorPos;
			
			float xspeed = 0.250f;
			float yspeed = 0.250f;

			float value = xspeed * (float)-(cursorPos.x - prevPos.x);

			if (abs(value) > 0.05f)
			{
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * value * TimeManager::DeltaTime());
			}

			value = yspeed * (float)-(cursorPos.y - prevPos.y);

			if (abs(value) > 0.05f)
			{
				float angle = yspeed * value * TimeManager::DeltaTime();

				float dot = glm::dot(cam->GetForwardVector(), { 0.0f, 1.0f, 0.0f });

				if (dot > -0.9f && angle < 0)
				{
					cam->Rotate(cam->GetRightVector(), angle);
				}

				if (dot < 0.9f && angle > 0)
				{
					cam->Rotate(cam->GetRightVector(), angle);
				}
			}

			ClientSend("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), false);
			
			prevPos = cursorPos;
		});

	gamepadLookX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.05f)
			{
				float xspeed = -10.0f;
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * value * TimeManager::DeltaTime());

				ClientSend("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), false);
			}


		});

	gamepadLookY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.05f)
			{
				float yspeed = -10.0f;
				float angle = yspeed * value * TimeManager::DeltaTime();

				float dot = glm::dot(cam->GetForwardVector(), { 0.0f, 1.0f, 0.0f });

				if (dot > -0.9f && angle < 0)
				{
					cam->Rotate(cam->GetRightVector(), angle);
				}

				if (dot < 0.9f && angle > 0)
				{
					cam->Rotate(cam->GetRightVector(), angle);
				}

				ClientSend("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), false);
			}
		});

	gamepadWalkX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.01f)
			{
				float xspeed = -10.0f;
				ClientSend("DispX " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(glm::normalize(characterGraphics->GetTransform()[0]) * value * xspeed * TimeManager::DeltaTime()), false);
			}
		});

	gamepadWalkY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.01f)
			{
				float xspeed = -10.0f;
				ClientSend("DispY " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(glm::normalize(characterGraphics->GetTransform()[2]) * value * xspeed * TimeManager::DeltaTime()), false);
			}
		});

	gamepadJump = new std::function<void(int)>([this](int button)
		{
			ClientSend("Jump " + std::to_string(jumpPacketNumber++) + " ", false);
		});

	keyboardMove = new std::function<void(int)>([this](int key)
		{
			float xspeed = 10.0f;
			glm::vec3 disp(0.0f);
			
			switch (key)
			{
			case KEY_W:
				disp = glm::vec3(glm::normalize(characterGraphics->GetTransform()[2]) * xspeed * TimeManager::DeltaTime());
				ClientSend("DispY " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(disp), false);
				break;
			case KEY_A:
				disp = glm::vec3(glm::normalize(characterGraphics->GetTransform()[0]) * xspeed * TimeManager::DeltaTime());
				ClientSend("DispX " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(disp), false);
				break;
			case KEY_S:
				disp = -glm::vec3(glm::normalize(characterGraphics->GetTransform()[2]) * xspeed * TimeManager::DeltaTime());
				ClientSend("DispY " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(disp), false);
				break;
			case KEY_D:
				disp = -glm::vec3(glm::normalize(characterGraphics->GetTransform()[0]) * xspeed * TimeManager::DeltaTime());
				ClientSend("DispX " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(disp), false);
				break;
			default:
				break;
			}
		});

	keyboardJump = new std::function<void(int key)>([this](int key)
		{
			ClientSend("Jump " + std::to_string(jumpPacketNumber++) + " ");
		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, keyboardJump, "FPSCharacterJump");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_X, gamepadLookX, "FPSCharacterLook");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_Y, gamepadLookY, "FPSCharacterLook");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_X, gamepadWalkX, "FPSCharacterWalk");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_Y, gamepadWalkY, "FPSCharacterWalk");
	InputManager::RegisterCallbackForGamepadButton(KEY_PRESS, GAMEPAD_BUTTON_A, gamepadJump, "FPSCharacterJump");
	
}

void FPSPlayer::DeregisterInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, "FPSCharacterJump");
	InputManager::DeregisterCallbackForGamepadButton(KEY_PRESS, GAMEPAD_BUTTON_A, "FPSCharacterJump");

	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_X, "FPSCharacterLook");
	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_Y, "FPSCharacterLook");

	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_X, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_Y, "FPSCharacterWalk");

	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "FPSCharacterWalk");

	delete keyboardMove;

	delete gamepadJump;
	delete keyboardJump;

	delete gamepadLookX;
	delete gamepadLookY;
	delete gamepadWalkX;
	delete gamepadWalkY;

	delete whenCursorMove;
	delete editorEnable;
	delete editorDisable;
}

void FPSPlayer::RegisterEditorToggleCallbacks()
{
	editorEnable = new std::function<void()>([this]()
		{
			InputManager::EnableCursor("Engine");

			if (!hitBox->IsVisible())
			{
				hitBox->ToggleVisibility();
			}

			InputManager::EnableCursor("Engine");
		});

	editorDisable = new std::function<void()>([this]()
		{
			InputManager::DisableCursor("Engine");

			if (hitBox->IsVisible())
			{
				hitBox->ToggleVisibility();
			}

			InputManager::DisableCursor("Engine");
		});

	Editor::RegisterOnEditorEnable(editorEnable);
	Editor::RegisterOnEditorDisable(editorDisable);
}

void FPSPlayer::DeregisterEditorToggleCallbacks()
{
	Editor::DeregisterOnEditorEnable(editorEnable);
	Editor::DeregisterOnEditorDisable(editorDisable);
}

void FPSPlayer::OnSpawn()
{
	NetworkObject::OnSpawn();

	Scene* scene = SceneManager::GetRegisteredScene("Test");

	if (scene != nullptr)
	{
		scene->RegisterGameObject(this, "FPSPlayer:" + std::to_string(GetNetworkObjectID()));
	}

	if (NetworkManager::IsServer())
	{
		onClientDisconnect = new std::function<void(const std::string&)>([this](const std::string& IP)
			{
				if (GetSpawnerIP() == IP)
				{
					NetworkManager::Despawn(GetNetworkObjectID());
				}
			});

		NetworkManager::RegisterOnClientDisconnectFunction("FPSPlayer:" + std::to_string(GetNetworkObjectID()), onClientDisconnect);
	}
	else
	{
		//ClientSend(NetworkManager::ConvertVec3ToData(target));
	}
}

void FPSPlayer::OnDespawn()
{
}

void FPSPlayer::OnDataReceived(const std::string& data)
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

			if(std::stoi(packetID) >= lastPacket)
				positionToSet = NetworkManager::ConvertDataToVec3(updateData);
			else
			{
				Logger::Log("Lost position packet");
			}

			lastPacket = std::stoi(packetID);
		}
		else if (updateType == "Disp")
		{
			static unsigned int lastPacket = std::stoi(packetID);

			if (std::stoi(packetID) >= lastPacket)
				disp = NetworkManager::ConvertDataToVec3(updateData);
				
			lastPacket = std::stoi(packetID);
		}
		else if (updateType == "Target")
		{
			static unsigned int lastPacket = std::stoi(packetID);

			if (std::stoi(packetID) >= lastPacket)
				targetToSet = NetworkManager::ConvertDataToVec3(updateData);

			lastPacket = std::stoi(packetID);
		}
		else if (updateType == "Rotation")
		{
			static unsigned int lastPacket = std::stoi(packetID);

			if (std::stoi(packetID) >= lastPacket)
				rotationToSet = NetworkManager::ConvertDataToMat4(updateData);

			lastPacket = std::stoi(packetID);
		}
		else if (updateType == "CameraPosition")
		{
			static unsigned int lastPacket = std::stoi(packetID);

			if (std::stoi(packetID) >= lastPacket)
				camPositionToSet = NetworkManager::ConvertDataToVec3(updateData);

			lastPacket = std::stoi(packetID);
		}
	}
	else
	{
		if (updateType == "DispX")
		{
			controller->AddDisp(NetworkManager::ConvertDataToVec3(updateData));
		}
		else if (updateType == "DispY")
		{
			controller->AddDisp(NetworkManager::ConvertDataToVec3(updateData));
		}
		else if (updateType == "Jump")
		{
			if (!controller->IsFalling())
			{
				controller->Jump(100.0f, 0.7f);
			}
		}
		else if (updateType == "Target")
		{
			static unsigned int lastPacket = std::stoi(packetID);

			if (std::stoi(packetID) >= lastPacket)
				targetToSet = NetworkManager::ConvertDataToVec3(updateData);

			lastPacket = std::stoi(packetID);
		}
	}
}

void FPSPlayer::OnServerSpawnConfirmation(const std::string& IP)
{
	NetworkObject::OnServerSpawnConfirmation(IP);
}

void FPSPlayer::OnClientSpawnConfirmation()
{
}
