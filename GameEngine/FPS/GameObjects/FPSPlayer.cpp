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

	hitBox = new AnimatedColliderComponent(characterGraphics);
	hitBox->Update();

	AddComponent(hitBox, "AnimatedCollider");
	
	if (NetworkManager::IsServer())
	{
		controller = new CharacterControllerComponent("FPSPlayer" + std::to_string(GetNetworkObjectID()), 0.35f, 1.0f, characterGraphics->GetPosition());
		AddComponent(controller, "Controller");

		RegisterEditorToggleCallbacks();
	}

	cam = new CameraComponent("FPS");

	cam->SetPosition(hitBox->GetJointTransform("Head")[3] + glm::normalize(hitBox->GetJointTransform("Head")[0]) * 0.5f);
	cam->SetTarget({ 0.0f, 0.0f, 30.0f });
	cam->SetFOV(20.0f);
	cam->SetNear(0.01f);

	AddComponent(cam, "Camera");

	if (SpawnedFromLocalSpawnRequest())
	{
		RegisterInput();
	}
}

void FPSPlayer::Terminate()
{
	if (SpawnedFromLocalSpawnRequest())
	{
		DeregisterInput();
		RemoveComponent("Camera");
		delete cam;
	}

	if (NetworkManager::IsServer())
	{
		RemoveComponent("Controller");
		delete controller;
	}

	RemoveComponent("Graphics");
	RemoveComponent("AnimatedCollider");

	delete hitBox;
	delete characterGraphics;
}

void FPSPlayer::GameUpdate()
{
	hitBox->Update();

	cam->SetPosition(hitBox->GetJointTransform("Head")[3] + glm::normalize(hitBox->GetJointTransform("Head")[0]) * 0.5f);

	glm::vec3 camRight = cam->GetRightVector();
	glm::vec3 newForward = glm::normalize(glm::cross(camRight, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 newUp = glm::normalize(glm::cross(newForward, camRight));

	characterGraphics->SetRotation(glm::mat4(glm::vec4(-camRight, 0.0f), glm::vec4(newUp, 0.0f), glm::vec4(-newForward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

	if (NetworkManager::IsServer())
	{
		controller->Update();
		characterGraphics->SetPosition(controller->GetPosition());

		static float updateTime = 0.0f;
		updateTime += TimeManager::DeltaTime();

		if (updateTime >= 0.001f)
		{
			ServerSendAll("Position " + NetworkManager::ConvertVec3ToData(controller->GetPosition()), {}, false);
			ServerSendAll("Target " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), {}, false);
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
}

void FPSPlayer::RegisterInput()
{
	whenCursorMove = new std::function<void(const glm::vec2&)>([this](const glm::vec2& cursorPos)
		{
			static glm::vec2 prevPos = cursorPos;
			
			float xspeed = 1.0f;
			float yspeed = 1.0f;
			if (cursorPos.x != prevPos.x)
			{
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * (float)-(cursorPos.x - prevPos.x) * TimeManager::DeltaTime());
			}
			if (cursorPos.y != prevPos.y)
			{
				float angle = yspeed * (float)-(cursorPos.y - prevPos.y) * TimeManager::DeltaTime();

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
			
			prevPos = cursorPos;
		});

	gamepadLookX = new std::function<void(int, float)>([this](int axis, float value)
		{
			ClientSend("LookX " + std::to_string(value), false);
		});

	gamepadLookY = new std::function<void(int, float)>([this](int axis, float value)
		{
			ClientSend("LookY " + std::to_string(value), false);
		});

	gamepadWalkX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.01f)
			{
				float xspeed = -10.0f;
				ClientSend("Disp " + NetworkManager::ConvertVec3ToData(glm::normalize(characterGraphics->GetTransform()[0]) * value * xspeed * TimeManager::DeltaTime()), false);
			}
		});

	gamepadWalkY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.01f)
			{
				float xspeed = -10.0f;
				ClientSend("Disp " + NetworkManager::ConvertVec3ToData(glm::normalize(characterGraphics->GetTransform()[2]) * value * xspeed * TimeManager::DeltaTime()), false);
			}
		});

	gamepadJump = new std::function<void(int)>([this](int button)
		{
			ClientSend("Jump ", false);
		});

	keyboardMove = new std::function<void(int)>([this](int key)
		{
			//float xspeed = 10.0f;
			//glm::vec3 disp(0.0f);
			//
			//switch (key)
			//{
			//case KEY_W:
			//	disp = glm::vec3(glm::normalize(characterGraphics->GetTransform()[2]) * xspeed * TimeManager::DeltaTime());
			//	break;
			//case KEY_A:
			//	disp = glm::vec3(glm::normalize(characterGraphics->GetTransform()[0]) * xspeed * TimeManager::DeltaTime());
			//	break;
			//case KEY_S:
			//	disp = -glm::vec3(glm::normalize(characterGraphics->GetTransform()[2]) * xspeed * TimeManager::DeltaTime());
			//	break;
			//case KEY_D:
			//	disp = -glm::vec3(glm::normalize(characterGraphics->GetTransform()[0]) * xspeed * TimeManager::DeltaTime());
			//	break;
			//default:
			//	break;
			//}
			//
			//controller->AddDisp(disp);
		});

	keyboardJump = new std::function<void(int key)>([this](int key)
		{
			if (!controller->IsFalling())
			{
				ClientSend("Jump ");
			}
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
		});

	editorDisable = new std::function<void()>([this]()
		{
			InputManager::DisableCursor("Engine");

			if (hitBox->IsVisible())
			{
				hitBox->ToggleVisibility();
			}
		});

	Editor::RegisterOnEditorEnable(editorEnable);
	Editor::RegisterOnEditorDisable(editorDisable);
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

	for (const auto& character : data)
	{
		if (character == ' ')
		{
			break;
		}
		updateType += character;
	}

	std::string updateData;

	for (const auto& character : std::string(data.begin() + updateType.size() + 1, data.end()))
	{
		updateData += character;
	}

	if (!NetworkManager::IsServer())
	{
		if (updateType == "Position")
		{
			SetPosition(NetworkManager::ConvertDataToVec3(updateData));
		}
		else if (updateType == "Disp")
		{
			disp = NetworkManager::ConvertDataToVec3(updateData);
		}
		else if (updateType == "Target")
		{
			cam->SetTarget(NetworkManager::ConvertDataToVec3(updateData));
		}
	}
	else
	{
		if (updateType == "Disp")
		{
			controller->AddDisp(NetworkManager::ConvertDataToVec3(updateData));
		}
		else if (updateType == "Jump")
		{
			if (!controller->IsFalling())
			{
				controller->Jump(100.0f, 1.0f);
			}
		}
		else if (updateType == "LookX")
		{
			float value = std::stof(updateData);

			if (abs(value) > 0.05f)
			{
				float xspeed = -10.0f;
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * value * TimeManager::DeltaTime());
			}
		}
		else if (updateType == "LookY")
		{
			float value = std::stof(updateData);

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
			}
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
