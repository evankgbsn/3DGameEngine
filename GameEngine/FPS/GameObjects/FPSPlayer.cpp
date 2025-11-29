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

FPSPlayer::FPSPlayer() :
	GameObject("FPSPlayer")
{
	RegisterGameObjectClassType<FPSPlayer>(this);
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

	cam = new CameraComponent("FPS");

	cam->SetPosition(hitBox->GetJointTransform("Head")[3] + glm::normalize(hitBox->GetJointTransform("Head")[0]) * 0.5f);
	cam->SetFOV(20.0f);
	cam->SetNear(0.00001f);

	AddComponent(cam, "Camera");

	controller = new CharacterControllerComponent("FPSPlayer", 0.35f, 1.0f, characterGraphics->GetPosition());
	AddComponent(controller, "Controller");

	RegisterInput();

	RegisterEditorToggleCallbacks();
}

void FPSPlayer::Terminate()
{
	delete characterGraphics;
}

void FPSPlayer::GameUpdate()
{
	
	cam->SetPosition(hitBox->GetJointTransform("Head")[3] + glm::normalize(hitBox->GetJointTransform("Head")[0]) * 0.5f);

	glm::vec3 camRight = cam->GetRightVector();
	glm::vec3 newForward = glm::normalize(glm::cross(camRight, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 newUp = glm::normalize(glm::cross(newForward, camRight));

	characterGraphics->SetRotation(glm::mat4(glm::vec4(-camRight, 0.0f), glm::vec4(newUp, 0.0f), glm::vec4(-newForward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));


	InputManager::WhenCursorMoved(*whenCursorMove);

	characterGraphics->SetPosition(controller->GetPosition());

	hitBox->Update();
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
	cam->SetActive();
}

void FPSPlayer::RegisterInput()
{
	whenCursorMove = new std::function<void(const glm::vec2&)>([this](const glm::vec2& cursorPos)
		{
			static glm::vec2 prevPos = cursorPos;
			
			float xspeed = 0.0005f;
			float yspeed = 0.0005f;
			if (cursorPos.x != prevPos.x)
			{
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * (float)-(cursorPos.x - prevPos.x));
			}
			if (cursorPos.y != prevPos.y)
			{
				cam->Rotate(cam->GetRightVector(), yspeed * (float)-(cursorPos.y - prevPos.y));
			}
			
			prevPos = cursorPos;
		});

	gamepadLookX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.05f)
			{
				float xspeed = -10.0f;
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * value * TimeManager::DeltaTime());
			}
		});

	gamepadLookY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.05f)
			{
				float yspeed = -10.0f;
				cam->Rotate(cam->GetRightVector(), yspeed * value * TimeManager::DeltaTime());
			}
		});

	gamepadWalkX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.05f)
			{
				float xspeed = -1000.0f;
				//body->AddForce(glm::normalize(characterGraphics->GetTransform()[2]) * value * xspeed * TimeManager::DeltaTime());
			}
		});

	gamepadWalkY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.05f)
			{
				float yspeed = -1000.0f;
				//body->AddForce(glm::normalize(characterGraphics->GetTransform()[2]) * value * yspeed * TimeManager::DeltaTime());
			}
		});

	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_X, gamepadLookX, "FPSCharacterLook");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_Y, gamepadLookY, "FPSCharacterLook");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_X, gamepadWalkX, "FPSCharacterWalk");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_Y, gamepadWalkY, "FPSCharacterWalk");
}

void FPSPlayer::DeregisterInput()
{
	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_X, "FPSCharacterLook");
	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_Y, "FPSCharacterLook");

	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_X, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_Y, "FPSCharacterWalk");

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
