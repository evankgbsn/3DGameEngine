#include "FPSPlayer.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedAnimatedLit.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
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
#include "GameEngine/Math/Math.h"
#include "GameEngine/UI/Sprite.h"
#include "GameEngine/Renderer/Window/Window.h"
#include "GameEngine/Renderer/Window/WindowManager.h"
#include "GameEngine/GameObject/Component/GraphicsObjectLine.h"
#include "GameEngine/Physics/PhysicsManager.h"
#include "GameEngine/Math/Shapes/LineSegment3D.h"
#include "SpawnPoint.h"

#include <glm/gtc/matrix_transform.hpp>

#include <random>

FPSPlayer::FPSPlayer() :
	GameObject("FPSPlayer"),
	controller(nullptr),
	shotCast(nullptr)
{
	RegisterGameObjectClassType<FPSPlayer>(this);
	RegisterNetworkObjectClassType<FPSPlayer>(this);
}

FPSPlayer::~FPSPlayer()
{
}

bool FPSPlayer::IsLocal() const
{
	return SpawnedFromLocalSpawnRequest();
}

glm::mat4 FPSPlayer::GetWeaponTransform()
{
	if (ak12Graphics != nullptr)
	{
		return ak12Graphics->GetTransform();
	}

	return glm::mat4(1.0f);
}

void FPSPlayer::Initialize()
{
	if (SpawnedFromLocalSpawnRequest())
	{
		characterGraphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::GetModel("Character"), "Character", "CharacterSpec", "CharacterNormal");
		characterGraphics->SetClip("Idle");
		characterGraphics->InitializeAdditiveAnimation("LookUp");
		characterGraphics->InitializeAdditiveAnimation("LookDown");
		characterGraphics->SetShine(32.0f);
		characterGraphics->SetPosition({ 0.0f, 20.0f, 0.0f });
		characterGraphics->SetSpeed(1.0f);
		characterGraphics->SetRenderGraphics(false);

		hitBox = new AnimatedColliderComponent(characterGraphics);
		hitBox->Update();
		AddComponent(hitBox, "AnimatedCollider");

		AddComponent(characterGraphics, "Graphics");

		characterArmsGraphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::GetModel("CharacterArms"), "Character", "CharacterSpec", "CharacterNormal");
		characterArmsGraphics->SetClip("Idle");
		characterArmsGraphics->InitializeAdditiveAnimation("LookUp");
		characterArmsGraphics->InitializeAdditiveAnimation("LookDown");
		characterArmsGraphics->SetShine(32.0f);
		characterArmsGraphics->SetPosition({ 0.0f, 20.0f, 0.0f });
		characterArmsGraphics->SetSpeed(1.0f);
		characterArmsGraphics->SetRenderShadow(false);
		characterArmsGraphics->SetRenderReflection(false);
		AddComponent(characterArmsGraphics, "ArmsGraphics");

		glm::vec2 dimensions = Window::GetPrimaryMonitorDimensions();

		crosshair = new Sprite("Crosshair", { 0.5f, 0.5f }, { 0.000005f * dimensions.y , 0.000005f * dimensions.x});

		controller = new CharacterControllerComponent("FPSPlayer" + std::to_string(GetNetworkObjectID()), .60f, 1.40f, characterGraphics->GetPosition());
		controller->SetOwner(this);
		AddComponent(controller, "Controller");

		healthBar = new Sprite("HealthBar", glm::vec2( 0.05f, 0.1f ), glm::vec2( 0.0001f * dimensions.x, 0.000025f * dimensions.y ), glm::vec2(-1.0f, 0.0f));
	}
	else
	{
		characterGraphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::GetModel("Character"), "Character", "CharacterSpec", "CharacterNormal");
		characterGraphics->SetClip("Idle");
		characterGraphics->SetShine(32.0f);
		characterGraphics->InitializeAdditiveAnimation("LookUp");
		characterGraphics->InitializeAdditiveAnimation("LookDown");
		characterGraphics->SetPosition({ 0.0f, 20.0f, 0.0f });
		characterGraphics->SetSpeed(1.0f);

		AddComponent(characterGraphics, "Graphics");

		positionToSet = characterGraphics->GetPosition();

		hitBox = new AnimatedColliderComponent(characterGraphics);
		hitBox->Update();

		AddComponent(hitBox, "AnimatedCollider");

		if (NetworkManager::IsServer())
		{
			glm::vec3 position;
			glm::mat4 rotation;

			GetSpawnPositionAndRotation(position, rotation);

			characterGraphics->SetPosition(position);
			characterGraphics->SetRotation(rotation);

			controller = new CharacterControllerComponent("FPSPlayer" + std::to_string(GetNetworkObjectID()), .60f, 1.40f, characterGraphics->GetPosition());
			controller->SetOwner(this);
			AddComponent(controller, "Controller");
		}
	}

	ak12Graphics = new GraphicsObjectTexturedLit(ModelManager::GetModel("AK12"), "Character", "CharacterSpec", "CharacterNormal");
	ak12Graphics->SetShine(32.0f);

	AddComponent(ak12Graphics, "WeaponGraphics");

	cam = new CameraComponent("FPSCharacter:" + std::to_string(GetNetworkObjectID()));

	cam->SetPosition(hitBox->GetJointTransform("head.x")[3]);
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
		RemoveComponent("ArmsGraphics");

		delete characterArmsGraphics;

		delete crosshair;
		delete controller;
		delete healthBar;
		DeregisterInput();
	}

	RemoveComponent("AnimatedCollider");
	delete hitBox;

	RemoveComponent("Graphics");
	delete characterGraphics;

	if (NetworkManager::IsServer())
	{
		RemoveComponent("Controller");
		delete controller;
		delete onClientDisconnect;
	}

	RemoveComponent("Camera");
	delete cam;

	RemoveComponent("WeaponGraphics");
	delete ak12Graphics;
}

void FPSPlayer::GameUpdate()
{
	if (SpawnedFromLocalSpawnRequest())
	{
		if (!NetworkManager::IsServer())
		{
			SetPosition(positionToSet);
			newPositionFromServer.store(false);
		}
		else
		{
			characterGraphics->SetPosition(controller->GetFootPosition());
		}
	}
	else
	{
		if (!NetworkManager::IsServer())
		{
			SetPosition(footPositionToSet);
			newFootPositionFromServer.store(false);
		}

		characterGraphics->SetAdditiveAnimationTime("LookUp", additiveUpToSet);
		characterGraphics->SetAdditiveAnimationTime("LookDown", additiveDownToSet);
	}
	
	hitBox->Update();

	cam->SetPosition(hitBox->GetJointTransform("head.x")[3]);

	if (!SpawnedFromLocalSpawnRequest())
	{
		cam->SetTarget(targetToSet);

		if (NetworkManager::IsServer())
		{
			camUpdateTime += TimeManager::DeltaTime();

			if (camUpdateTime >= 0.001f)
			{
				ServerSendAll("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), {}, false);
				camUpdateTime = 0.0f;
			}

			lastTarget = cam->GetTarget();
		}
	}

	glm::vec3 camRight = cam->GetRightVector();
	glm::vec3 newForward = glm::normalize(glm::cross(camRight, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 newUp = glm::normalize(glm::cross(newForward, camRight));

	if (SpawnedFromLocalSpawnRequest())
	{
		controller->Update();

		characterArmsGraphics->SetTransform(characterGraphics->GetTransform());

		characterGraphics->SetRotation(glm::mat4(glm::vec4(-camRight, 0.0f), glm::vec4(newUp, 0.0f), glm::vec4(-newForward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

		glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

		glm::mat4 armRotation = glm::mat4(glm::vec4(camRight, 0.0f), glm::vec4(cam->GetUpVector(), 0.0f), -glm::vec4(cam->GetForwardVector(), 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		armRotation = glm::rotate(armRotation, glm::radians(180.0f), glm::vec3(up));

		glm::vec3 riflePositionOffset =
			(glm::vec3(glm::normalize(armRotation[0])) * -0.10f)
			+ (glm::vec3(glm::normalize(armRotation[2])) * .60f)
			+ (glm::vec3(glm::normalize(armRotation[1])) * -0.15f);

		glm::mat4 rightHandTransform = characterArmsGraphics->GetJointTransform("middle1.r");
		glm::mat4 leftHandTransform = characterArmsGraphics->GetJointTransform("middle1.l");

		glm::vec3 fromRtoL = glm::normalize(leftHandTransform[3] - rightHandTransform[3]);
		glm::vec3 newRight = glm::normalize(glm::cross(cam->GetUpVector(), fromRtoL));

		glm::mat4 weaponRot(1.0f);

		if (characterArmsGraphics->GetCurrentAnimation() == "AimRun" || characterArmsGraphics->GetFadeToClipName() == "AimRun")
		{
			weaponRot = { glm::vec4(newRight, 0.0f),
			glm::vec4(cam->GetUpVector(), 0.0f),
			glm::vec4(fromRtoL, 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
			};
		}
		else
		{
			weaponRot = { -glm::vec4(cam->GetRightVector(), 0.0f),
			glm::vec4(cam->GetUpVector(), 0.0f),
			glm::vec4(cam->GetForwardVector(), 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
			};
		}
		

		ak12Graphics->SetRotation(weaponRot);
		ak12Graphics->SetPosition(glm::vec3(rightHandTransform[3]));
	}
	else
	{
		characterGraphics->SetRotation(glm::mat4(glm::vec4(-camRight, 0.0f), glm::vec4(newUp, 0.0f), glm::vec4(-newForward, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		ak12Graphics->SetTransform(weaponPositionToSet);
	}

	if (NetworkManager::IsServer())
	{
		controller->Update();
		characterGraphics->SetPosition(controller->GetFootPosition());

		updateTime += TimeManager::DeltaTime();
		
		if (updateTime >= 0.001f)
		{
			ServerSendAll("Position " + std::to_string(positionPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(controller->GetPosition()), {}, false);
			ServerSendAll("FootPosition " + std::to_string(footPositionPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(controller->GetFootPosition()), {}, false);

			updateTime = 0.0f;
		}

		lastPosition = characterGraphics->GetPosition();
	}
	else if(SpawnedFromLocalSpawnRequest())
	{
		updateTime += TimeManager::DeltaTime();

		if (updateTime >= 0.001f)
		{
			ClientSend("WeaponPosition " + std::to_string(weaponPositionPacketNumber++) + " " + NetworkManager::ConvertMat4ToData(GetWeaponTransform()), false);
			ClientSend("AdditiveAnimationUp " + std::to_string(lookUpPacketNumber++) + " " + std::to_string(characterGraphics->GetAdditiveAnimationTime("LookUp")), false);
			ClientSend("AdditiveAnimationDown " + std::to_string(lookDownPacketNumber++) + " " + std::to_string(characterGraphics->GetAdditiveAnimationTime("LookDown")), false);
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

	float speed = 1.0f;
	float additiveAnimationTime = 0.0f;
}

void FPSPlayer::Load()
{
	if (!ModelManager::ModelLoaded("AK12"))
	{
		ModelManager::LoadModel("AK12", "Assets/Model/AK12.gltf", false);
	}

	if (!ModelManager::ModelLoaded("Character"))
	{
		ModelManager::LoadModel("Character", "Assets/Model/FPSCharacter.gltf", false);
	}

	if (!TextureManager::TextureLoaded("Character"))
	{
		TextureManager::LoadTexture("Assets/Texture/Black.png", "Character");
	}

	if (!TextureManager::TextureLoaded("CharacterSpec"))
	{
		TextureManager::LoadTexture("Assets/Texture/WarehouseGroundTextureSpec.png", "CharacterSpec");
	}

	if (!TextureManager::TextureLoaded("CharacterNormal"))
	{
		TextureManager::LoadTexture("Assets/Texture/Normal.png", "CharacterNormal");
	}

	if (SpawnedFromLocalSpawnRequest())
	{
		if (!ModelManager::ModelLoaded("CharacterArms"))
		{
			ModelManager::LoadModel("CharacterArms", "Assets/Model/FPSCharacterArms.gltf", false);
		}

		if (!ModelManager::ModelLoaded("CharacterLegs"))
		{
			ModelManager::LoadModel("CharacterLegs", "Assets/Model/FPSCharacterLegs.gltf", false);
		}

		if (!TextureManager::TextureLoaded("Crosshair"))
		{
			TextureManager::LoadTexture("Assets/Texture/Crosshair.png", "Crosshair");
		}

		if (!TextureManager::TextureLoaded("HealthBar"))
		{
			TextureManager::LoadTexture("Assets/Texture/Red.png", "HealthBar");
		}

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
	if (controller != nullptr)
	{
		controller->SetPosition(pos);
		characterGraphics->SetPosition(controller->GetFootPosition());
	}
	else
	{
		characterGraphics->SetPosition(pos);
	}
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
	static std::function<void(int)> escapeCursor = [](int keyCode)
		{
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

	animationStartCallback = new std::function<void(const std::string&)>([this](const std::string&)
		{
			if (characterGraphics->isFading())
			{
				ClientSend("AnimationClip " + std::to_string(animationClipPacketNumber++) + " " + characterGraphics->GetFadeToClipName(), false);
			}
			else
			{
				ClientSend("AnimationClip " + std::to_string(animationClipPacketNumber++) + " " + characterGraphics->GetCurrentAnimation(), false);
			}

		});

	characterGraphics->RegisterAnimationStartCallback("NetworkSync", animationStartCallback);

	animationStopCallback = new std::function<void(const std::string&)>([this](const std::string&)
		{
		});

	characterGraphics->RegisterAnimationStartCallback("NetworkSync", animationStopCallback);

	whenCursorMove = new std::function<void(const glm::vec2&)>([this](const glm::vec2& cursorPos)
		{
			static glm::vec2 prevPos = cursorPos;
			
			float xspeed = 1.0f;
			float yspeed = 1.0f;

			float value = xspeed * (float)-(cursorPos.x - prevPos.x);

			if (abs(value) > 0.005f)
			{
				cam->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), value * TimeManager::DeltaTime());
				ClientSend("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), false);
			}

			value = yspeed * (float)-(cursorPos.y - prevPos.y);

			if (abs(value) > 0.005f)
			{
				float angle = value * TimeManager::DeltaTime();

				float dot = glm::dot(cam->GetForwardVector(), { 0.0f, 1.0f, 0.0f });

				if (dot > -0.9f && angle < 0)
				{
					cam->Rotate(cam->GetRightVector(), angle);
				}

				if (dot < 0.9f && angle > 0)
				{
					cam->Rotate(cam->GetRightVector(), angle);
				}

				if (dot >= 0.0f)
				{
					characterGraphics->SetAdditiveAnimationTime("LookDown", 0.0f);
					characterGraphics->SetAdditiveAnimationTime("LookUp", Math::ChangeRange(0.0f, 0.90f, 0.0f, 1.0f, dot));

					characterArmsGraphics->SetAdditiveAnimationTime("LookDown", 0.0f);
					characterArmsGraphics->SetAdditiveAnimationTime("LookUp", Math::ChangeRange(0.0f, 0.90f, 0.0f, 1.0f, dot));
				}
				else
				{
					characterGraphics->SetAdditiveAnimationTime("LookUp", 0.0f);
					characterGraphics->SetAdditiveAnimationTime("LookDown", 1.0f - Math::ChangeRange(-0.90f, 0.0f, 0.0f, 1.0f, dot));

					characterArmsGraphics->SetAdditiveAnimationTime("LookUp", 0.0f);
					characterArmsGraphics->SetAdditiveAnimationTime("LookDown", 1.0f - Math::ChangeRange(-0.90f, 0.0f, 0.0f, 1.0f, dot));
				}

				ClientSend("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), false);
			}
			
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

				if (dot >= 0.0f)
				{
					characterGraphics->SetAdditiveAnimationTime("LookDown", 0.0f);
					characterGraphics->SetAdditiveAnimationTime("LookUp", Math::ChangeRange(0.0f, 0.90f, 0.0f, 1.0f, dot));

					characterArmsGraphics->SetAdditiveAnimationTime("LookDown", 0.0f);
					characterArmsGraphics->SetAdditiveAnimationTime("LookUp", Math::ChangeRange(0.0f, 0.90f, 0.0f, 1.0f, dot));
				}
				else
				{
					characterGraphics->SetAdditiveAnimationTime("LookUp", 0.0f);
					characterGraphics->SetAdditiveAnimationTime("LookDown", 1.0f - Math::ChangeRange(-0.90f, 0.0f, 0.0f, 1.0f, dot));

					characterArmsGraphics->SetAdditiveAnimationTime("LookUp", 0.0f);
					characterArmsGraphics->SetAdditiveAnimationTime("LookDown", 1.0f - Math::ChangeRange(-0.90f, 0.0f, 0.0f, 1.0f, dot));
				}

				ClientSend("Target " + std::to_string(targetPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(cam->GetTarget()), false);
			}
		});

	gamepadWalkX = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.01f)
			{
				float xspeed = -10.0f;
				controller->AddDisp(glm::normalize(characterGraphics->GetTransform()[0]) * value * xspeed * TimeManager::DeltaTime());
				ClientSend("DispX " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(glm::normalize(characterGraphics->GetTransform()[0]) * value * xspeed * TimeManager::DeltaTime()), false);
			}
		});

	gamepadWalkY = new std::function<void(int, float)>([this](int axis, float value)
		{
			if (abs(value) > 0.01f)
			{
				float xspeed = -10.0f;

				if (value < 0.0f && !walking)
				{
					walking = true;
					if (characterGraphics->GetCurrentAnimation() != "AimRun" && characterGraphics->GetFadeToClipName() != "AimRun")
					{
						characterGraphics->FadeAnimationTo("AimRun", 0.05f);
					}

					if (characterArmsGraphics->GetCurrentAnimation() != "AimRun" && characterArmsGraphics->GetFadeToClipName() != "AimRun")
					{
						characterArmsGraphics->FadeAnimationTo("AimRun", 0.05f);
					}
				}

				controller->AddDisp(glm::normalize(characterGraphics->GetTransform()[2]) * value * xspeed * TimeManager::DeltaTime());
				ClientSend("DispY " + std::to_string(dispPacketNumber++) + " " + NetworkManager::ConvertVec3ToData(glm::normalize(characterGraphics->GetTransform()[2]) * value * xspeed * TimeManager::DeltaTime()), false);
			} 
			else if (walking)
			{
				walking = false;
				if (characterGraphics->GetCurrentAnimation() != "Idle" && characterGraphics->GetFadeToClipName() != "Idle")
				{
					characterGraphics->FadeAnimationTo("Idle", 0.05f);
					characterArmsGraphics->FadeAnimationTo("Idle", 0.05f);
				}
			}
		});

	gamepadShoot = new std::function<void(int, float)>([this](int axis, float value)
		{
			static bool shooting = false;

			if (value > 0.0f && !shooting)
			{
				if (TimeManager::SecondsSinceStart() - lastShotTime > 0.5f)
				{
					characterArmsGraphics->FadeAnimationTo("RifleRecoil", 0.1f);
					characterArmsGraphics->SetSpeed(5.0f);

					shooting = true;

					// Projectile.
					//static std::function<void(NetworkObject* obj)> callback = [](NetworkObject* obj) {};
					//NetworkManager::Spawn("AK12Bullet", &callback);
					////ClientSend("Shoot " + std::to_string(shootPacketNumber++) + " ");
					//lastShotTime = TimeManager::SecondsSinceStart();

					// Hit-Scan.
					ClientSend("Shoot " + std::to_string(shootPacketNumber++) + " ");

					lastShotTime = TimeManager::SecondsSinceStart();
				}
			}
			else if(shooting)
			{
				shooting = false;
				characterArmsGraphics->FadeAnimationTo("Idle", 0.1f);
				characterArmsGraphics->SetSpeed(1.0f);
			}
		});

	gamepadJump = new std::function<void(int)>([this](int button)
		{
			ClientSend("Jump " + std::to_string(jumpPacketNumber++) + " ", false);

			if (!controller->IsFalling())
			{
				controller->Jump(75.0f);
			}
		});

	keyboardMove = new std::function<void(int)>([this](int key)
		{
			float xspeed = 10.0f;
			glm::vec3 disp(0.0f);

			Window* window = WindowManager::GetWindow("Engine");

			bool isWPressed = false;
			bool isAPressed = false;
			bool isSPressed = false;
			bool isDPressed = false;

			if (window != nullptr)
			{
				isWPressed = KEY_PRESSED == window->GetKey(KEY_W);
				isAPressed = KEY_PRESSED == window->GetKey(KEY_A);
				isSPressed = KEY_PRESSED == window->GetKey(KEY_S);
				isDPressed = KEY_PRESSED == window->GetKey(KEY_D);
			}

			if (isWPressed && isAPressed || isWPressed && isDPressed)
			{
				xspeed /= 2.0f;
			}
			else if (isSPressed && isAPressed || isSPressed && isDPressed)
			{
				xspeed /= 2.0f;
			}
			else if (isAPressed && isWPressed || isAPressed && isSPressed)
			{
				xspeed /= 2.0f;
			}
			else if (isDPressed && isWPressed || isDPressed && isSPressed)
			{
				xspeed /= 2.0f;
			}

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

			controller->AddDisp(disp);
		});

	keyboardJump = new std::function<void(int key)>([this](int key)
		{
			ClientSend("Jump " + std::to_string(jumpPacketNumber++) + " ");
			
			if (!controller->IsFalling())
			{
				controller->Jump(75.0f);
			}
		});

	keyboardShoot = new std::function<void(int button)>([this](int button)
		{
			if (TimeManager::SecondsSinceStart() - lastShotTime > 0.5f)
			{
				characterArmsGraphics->FadeAnimationTo("RifleRecoil", 0.1f);
				characterArmsGraphics->SetSpeed(5.0f);

				// Projectile.
				//static std::function<void(NetworkObject* obj)> callback = [](NetworkObject* obj) {};
				//NetworkManager::Spawn("AK12Bullet", &callback);
				////ClientSend("Shoot " + std::to_string(shootPacketNumber++) + " ");
				//lastShotTime = TimeManager::SecondsSinceStart();

				// Hit-Scan.
				ClientSend("Shoot " + std::to_string(shootPacketNumber++) + " ");

				lastShotTime = TimeManager::SecondsSinceStart();
			}
		});

	keyboardShootRelease = new std::function<void(int button)>([this](int button)
		{
			characterArmsGraphics->FadeAnimationTo("Idle", 0.1f);
			characterArmsGraphics->SetSpeed(1.0f);
		});

	keyboardMovePress = new std::function<void(int key)>([this](int key)
		{
			switch (key)
			{
			case KEY_W:
				if (characterGraphics->GetCurrentAnimation() != "AimRun" && characterGraphics->GetFadeToClipName() != "AimRun")
				{
					characterGraphics->FadeAnimationTo("AimRun", 0.05f);
				}

				if (characterArmsGraphics->GetCurrentAnimation() != "AimRun" && characterArmsGraphics->GetFadeToClipName() != "AimRun")
				{
					characterArmsGraphics->FadeAnimationTo("AimRun", 0.05f);
				}
				wPressed = true;
				break;
			case KEY_A:
				aPressed = true;
				break;
			case KEY_S:
				sPressed = true;
				break;
			case KEY_D:
				dPressed = true;
				break;
			default:
				break;
			}
		});

	keyboardMoveRelease = new std::function<void(int key)>([this](int key)
		{
			switch (key)
			{
			case KEY_W:
				wPressed = false;
				break;
			case KEY_A:
				aPressed = false;
				break;
			case KEY_S:
				sPressed = false;
				break;
			case KEY_D:
				dPressed = false;
				break;
			default:
				break;
			}

			std::string log = "release ";


			if (!wPressed && !aPressed && !sPressed && !dPressed && !InputManager::IsGamepadConnected())
			{
				if (characterGraphics->GetCurrentAnimation() != "Idle" && characterGraphics->GetFadeToClipName() != "Idle")
				{
					characterGraphics->FadeAnimationTo("Idle", 0.05f);
					characterArmsGraphics->FadeAnimationTo("Idle", 0.05f);
				}
				
				log += std::string("Fading");
			}

		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, keyboardMovePress, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, keyboardMovePress, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_S, keyboardMovePress, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_D, keyboardMovePress, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASED, KEY_W, keyboardMoveRelease, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASED, KEY_A, keyboardMoveRelease, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASED, KEY_S, keyboardMoveRelease, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASED, KEY_D, keyboardMoveRelease, "FPSCharacterWalk");

	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESSED, MOUSE_BUTTON_1, keyboardShoot, "FPSCharacterShoot");
	InputManager::RegisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_1, keyboardShootRelease, "FPSCharacterShootRelease");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, keyboardJump, "FPSCharacterJump");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, keyboardMove, "FPSCharacterWalk");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_X, gamepadLookX, "FPSCharacterLook");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_Y, gamepadLookY, "FPSCharacterLook");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_X, gamepadWalkX, "FPSCharacterWalk");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_Y, gamepadWalkY, "FPSCharacterWalk");
	InputManager::RegisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_TRIGGER, gamepadShoot, "FPSCharacterShoot");
	InputManager::RegisterCallbackForGamepadButton(KEY_PRESS, GAMEPAD_BUTTON_A, gamepadJump, "FPSCharacterJump");
	
}

void FPSPlayer::DeregisterInput()
{
	delete animationStartCallback;
	delete animationStopCallback;

	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_W, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_A, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_S, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_D, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASED, KEY_W, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASED, KEY_A, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASED, KEY_S, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASED, KEY_D, "FPSCharacterWalk");


	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESSED, MOUSE_BUTTON_1, "FPSCharacterShoot");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_1, "FPSCharacterShoot");

	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, "FPSCharacterJump");
	InputManager::DeregisterCallbackForGamepadButton(KEY_PRESS, GAMEPAD_BUTTON_A, "FPSCharacterJump");

	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_X, "FPSCharacterLook");
	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_Y, "FPSCharacterLook");

	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_X, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_LEFT_Y, "FPSCharacterWalk");

	InputManager::DeregisterCallbackForGamepadAxis(GAMEPAD_AXIS_RIGHT_TRIGGER, "FPSCharacterShoot");

	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "FPSCharacterWalk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "FPSCharacterWalk");

	delete keyboardMove;
	delete keyboardShoot;
	delete keyboardShootRelease;

	delete gamepadJump;
	delete keyboardJump;

	delete gamepadLookX;
	delete gamepadLookY;
	delete gamepadWalkX;
	delete gamepadWalkY;

	delete gamepadShoot;

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

glm::mat4 FPSPlayer::ConvertRightHandTransformToWeaponTransform() const
{
	glm::mat4 handTransform = hitBox->GetJointTransform("middle1.r");


	glm::vec4 weaponup = -handTransform[0];
	glm::vec4 weaponforward = handTransform[1];
	glm::vec4 weaponright = handTransform[2];

	weaponright = glm::vec4(glm::normalize(glm::cross(glm::vec3(weaponup), glm::vec3(weaponforward))), 0.0f);

	return glm::mat4(weaponright, weaponup, weaponforward, glm::vec4(glm::vec3(handTransform[3]), 1.0f));
}

void FPSPlayer::Shoot()
{
	glm::vec3 origin = cam->GetPosition();
	glm::vec3 target = origin + cam->GetForwardVector() * 100.0f;
	glm::vec3 direction = cam->GetForwardVector();

	if (shotCast == nullptr)
	{
		shotCast = new GraphicsObjectLine(origin, target, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		shotCast->SetStart(origin);
		shotCast->SetEnd(target);
	}

	Scene* scene = SceneManager::GetRegisteredScene("Test");

	if (scene != nullptr)
	{
		const std::unordered_map<std::string, GameObject*> objects = scene->GetGameObjects();

		for (const auto& object : objects)
		{
			FPSPlayer* player = dynamic_cast<FPSPlayer*>(object.second);

			if (player != nullptr)
			{
				LineSegment3D line(origin, origin + direction * 1000.0f);
				std::string outBoxName;
				glm::vec3 outHit;

				const unsigned int maxHits = 10;
				physx::PxRaycastHit hits[maxHits];
				physx::PxRaycastBuffer outHits(hits, maxHits);

				bool status = PhysicsManager::Raycast(origin, direction, 1000.0f, outHits);

				bool hit = player->hitBox->Intersect(line, outBoxName, outHit);

				auto dealDamage = [this, player, outBoxName]()
					{
						float damage = 0.0f;

						if (outBoxName == "head.x")
						{
							damage = 100.0f;
						}
						else if (outBoxName == "spine_01.x")
						{
							damage = 20.0f;
						}
						else if (outBoxName == "spine_02.x")
						{
							damage = 30.0f;
						}
						else if (outBoxName == "spine_03.x")
						{
							damage = 40.0f;
						}
						else
						{
							damage = 10.0f;
						}

						Logger::Log(GetName() + " hit " + player->GetName() + " : " + outBoxName);

						player->ServerSend(player->GetSpawnerIP(), "Damage " + std::to_string(damagePacketNumber++) + " " + std::to_string(damage));
					};

				if (hit)
				{
					if (outHits.hasBlock)
					{
						if (glm::length(origin - outHit) < outHits.block.distance)
						{
							dealDamage();
						}
					}
					else
					{
						dealDamage();
					}
					
				}
			}
		}
	}
}

void FPSPlayer::GetSpawnPositionAndRotation(glm::vec3& position, glm::mat4& rotation)
{
	std::vector<SpawnPoint*> spawnPoints;

	for (const auto& object : GetOwningScene()->GetGameObjects())
	{
		SpawnPoint* point = dynamic_cast<SpawnPoint*>(object.second);

		if (point != nullptr)
		{
			spawnPoints.push_back(point);
		}
	}

	// Seed the random number generator with the current time
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	// Shuffle the elements
	std::shuffle(spawnPoints.begin(), spawnPoints.end(), generator);

	position = spawnPoints.front()->GetPosition();
	rotation = spawnPoints.front()->GetRotation();
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
}

void FPSPlayer::OnDespawn()
{
	NetworkManager::DeregisterOnClientDisconnectFunction("FPSPlayer:" + std::to_string(GetNetworkObjectID()));

	NetworkObject::OnDespawn();
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
			if (std::stoi(packetID) >= lastPositionPacketNumber)
			{
				positionToSet = NetworkManager::ConvertDataToVec3(updateData);
				newPositionFromServer = true;
			}
			else
			{
				Logger::Log("Lost position packet");
			}

			lastPositionPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "WeaponPosition")
		{
			weaponPositionToSet = NetworkManager::ConvertDataToMat4(updateData);
		}
		else if (updateType == "FootPosition")
		{
			if (std::stoi(packetID) >= lastFootPositionPacketNumber)
			{
				footPositionToSet = NetworkManager::ConvertDataToVec3(updateData);
				newFootPositionFromServer = true;
			}
			else
			{
				Logger::Log("Lost foot position packet");
			}

			lastFootPositionPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "Target")
		{
			if (std::stoi(packetID) >= lastTargetPacketNumber)
			{
				targetToSet = NetworkManager::ConvertDataToVec3(updateData);
			}
			else
			{
				Logger::Log("Lost target packet");
			}

			lastTargetPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "AdditiveAnimationUp")
		{
			if (std::stoi(packetID) >= lastAdditiveAnimationUpPacketNumber)
			{
				additiveUpToSet = std::stof(updateData);
			}
			else
			{
				Logger::Log("Lost additiveAnimationUp packet");
			}

			lastAdditiveAnimationUpPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "AdditiveAnimationDown")
		{
			if (std::stoi(packetID) >= lastAdditiveAnimationDownPacketNumber)
			{
				additiveDownToSet = std::stof(updateData);
			}
			else
			{
				Logger::Log("Lost additiveAnimationDown packet");
			}

			lastAdditiveAnimationDownPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "AnimationClip")
		{
			if (std::stoi(packetID) >= lastAnimationClipPacketNumber)
			{
				if (characterGraphics->GetCurrentAnimation() != updateData && characterGraphics->GetFadeToClipName() != updateData)
				{
					characterGraphics->SetClip(updateData);
				}
			}
			else
			{
				Logger::Log("Lost animation clip packet");
			}

			lastAnimationClipPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "Damage")
		{
			if (std::stoi(packetID) >= lastDamagePacketNumber)
			{
				health -= std::stof(updateData);

				if (health < 0.0f)
				{
					health = 0.0f;
				}

				healthBar->SetScale(healthBar->GetScale().x * (health * 0.01f), healthBar->GetScale().y);

				if (healthBar->GetScale().x <= 0.01f && shouldDie == false)
				{
					shouldDie = true;

					NetworkManager::Despawn(GetNetworkObjectID());

					static std::function<void(NetworkObject*)> callback = [this](NetworkObject* spawnedObject)
						{
						};

					NetworkManager::Spawn("FPSPlayer", &callback);
					//SceneManager::EndScene("Test");
					//SceneManager::TerminateScene("Test");
					//SceneManager::InitializeScene("Test");
					//SceneManager::GetRegisteredScene("Test")->Deserialize("Assets/Scenes/FPS.xml");
					//SceneManager::StartScene("Test");
				}
			}
			else
			{
				Logger::Log("Lost damage packet");
			}

			lastDamagePacketNumber = std::stoi(packetID);
		}
	}
	else
	{
		if (updateType == "DispX")
		{
			//if (characterGraphics->GetCurrentAnimation() != "AimRun")
			//{
			//	characterGraphics->FadeAnimationTo("AimRun", 0.5f);
			//}
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
				controller->Jump(75.0f);
			}
		}
		else if (updateType == "Target")
		{
			if (std::stoi(packetID) >= lastTargetPacketNumber)
				targetToSet = NetworkManager::ConvertDataToVec3(updateData);

			lastTargetPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "Shoot")
		{
			if (std::stoi(packetID) >= lastShootPacketNumber)
			{
				Shoot();
			}

			lastShootPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "WeaponPosition")
		{
			if (std::stoi(packetID) >= lastWeaponPositionPacketNumber)
			{
				weaponPositionToSet = NetworkManager::ConvertDataToMat4(updateData);

				ServerSendAll("WeaponPosition " + std::to_string(weaponPositionPacketNumber++) + " " + NetworkManager::ConvertMat4ToData(GetWeaponTransform()), { GetSpawnerIP() }, false);
			}

			lastWeaponPositionPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "AdditiveAnimationUp")
		{
			if (std::stoi(packetID) >= lastAdditiveAnimationUpPacketNumber)
			{
				additiveUpToSet = std::stof(updateData);

				ServerSendAll("AdditiveAnimationUp " + std::to_string(additiveAnimationUpPacketNumber++) + " " + updateData, {GetSpawnerIP()}, false);
			}

			lastAdditiveAnimationUpPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "AdditiveAnimationDown")
		{
			if (std::stoi(packetID) >= lastAdditiveAnimationDownPacketNumber)
			{
				additiveDownToSet = std::stof(updateData);

				ServerSendAll("AdditiveAnimationDown " + std::to_string(additiveAnimationDownPacketNumber++) + " " + updateData, { GetSpawnerIP() }, false);
			}

			lastAdditiveAnimationDownPacketNumber = std::stoi(packetID);
		}
		else if (updateType == "AnimationClip")
		{
			if (std::stoi(packetID) >= lastAnimationClipPacketNumber)
			{
				if (characterGraphics->GetCurrentAnimation() != updateData && characterGraphics->GetFadeToClipName() != updateData)
				{
					characterGraphics->SetClip(updateData);

					ServerSendAll("AnimationClip " + std::to_string(animationClipPacketNumber++) + " " + updateData, { GetSpawnerIP() }, false);
				}
			}
			else
			{
				Logger::Log("Lost animation clip packet");
			}

			lastAnimationClipPacketNumber = std::stoi(packetID);
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
