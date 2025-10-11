#include "SurvivalCharacter.h"


#include "GameEngine/GameObject/Component/GraphicsObjectTexturedAnimatedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/GameObject/Component/AnimatedColliderComponent.h"
#include "GameEngine/GameObject/Component/OrientedBoundingBoxComponent.h"
#include "GameEngine/Math/Shapes/LineSegment3D.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "SurvivalTerrain.h"
#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/Renderer/Camera/Camera.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Collision/AxisAlignedBoundingBoxWithVisualization.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Editor/Editor.h"
#include "SurvivalWater.h"
#include "SurvivalTree.h"
#include "GameEngine/Networking/NetworkManager.h"

#include <glm/gtx/transform.hpp>

#define CHARACTER_GRAPHICS_MODEL_NAME std::string("Woman")
#define CHARACTER_GRAPHICS_DIFFUSE_NAME std::string("Woman")
#define CHARACTER_GRAPHCIS_SPECULAR_NAME std::string("Woman")


SurvivalCharacter::SurvivalCharacter() :
	GameObject(std::string("SurvivalCharacter")),
	characterGraphics(nullptr),
	characterCamera(nullptr),
	characterCollider(nullptr),
	clickToMove(nullptr),
	target({0.0f, 0.0f, 0.0f}),
	walkSpeed(10.0f),
	cameraOrbit(nullptr),
	cameraDistance(20.0f),
	onEditorDisable(nullptr),
	onEditorEnable(nullptr)

{
	RegisterGameObjectClassType<SurvivalCharacter>(this);
	RegisterNetworkObjectClassType<SurvivalCharacter>(this);
}

SurvivalCharacter::~SurvivalCharacter()
{
	
}

void SurvivalCharacter::OnSpawn()
{
	NetworkObject::OnSpawn();

	Scene* scene = SceneManager::GetLoadedScene("SurvivalScene");

	if (scene != nullptr)
	{
		scene->RegisterGameObject(this, "SurvivalCharacter:" + std::to_string(GetNetworkObjectID()));
	}

	if (NetworkManager::IsServer())
	{
		onClientDisconnect = new std::function<void(const std::string&)>([this](const std::string& IP)
			{
				NetworkManager::Despawn(GetNetworkObjectID());
			});

		NetworkManager::RegisterOnClientDisconnectFunction("SurvivalCharacter", onClientDisconnect);
	}
	else
	{
	}

	currentTranslationVector = glm::vec3(0.0f);
}

void SurvivalCharacter::OnDespawn()
{
	if (NetworkManager::IsServer())
	{
		NetworkManager::DeregisterOnClientDisconnectFunction("SurvivalCharacter");
	}

	NetworkObject::OnDespawn();
}

void SurvivalCharacter::OnDataReceived(const std::string& data)
{
	NetworkObject::OnDataReceived(data);

	if (!NetworkManager::IsServer())
	{
		receivedPosition = NetworkManager::ConvertDataToVec3(data);
	}
	else
	{
		target = NetworkManager::ConvertDataToVec3(data);
	}
}

void SurvivalCharacter::OnServerSpawnConfirmation(const std::string& IP)
{
	NetworkObject::OnServerSpawnConfirmation(IP);
	ServerSend(IP, NetworkManager::ConvertVec3ToData(GetPosition()));
}

void SurvivalCharacter::Initialize()
{
	characterGraphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::GetModel(CHARACTER_GRAPHICS_MODEL_NAME), TextureManager::GetTexture(CHARACTER_GRAPHICS_DIFFUSE_NAME), TextureManager::GetTexture(CHARACTER_GRAPHCIS_SPECULAR_NAME));
	AddComponent(characterGraphics, "CharacterGraphics");
	
	characterGraphics->SetShine(32.0f);
	characterGraphics->SetClip(0);
	characterGraphics->SetSpeed(1.0f);
	characterGraphics->SetPosition({ 0.0f, 0.0f, 0.0f });
	
	if (!NetworkManager::IsServer() && SpawnedFromLocalSpawnRequest())
	{
		characterCamera = new CameraComponent("CharacterCamera");
		AddComponent(characterCamera, "CharacterCamera");

		cameraTarget = glm::vec3(0.0f, 0.0f, 10.0f);
		cameraPosition = glm::vec3(0.0f, 7.0f, -10.0f);

		characterCamera->SetPosition(characterGraphics->GetPosition() + cameraPosition);
		characterCamera->SetTarget(characterGraphics->GetPosition());

		SetupMovement();
		SetupCameraMovement();
	}
	
	characterCollider = new AnimatedColliderComponent(characterGraphics);
	AddComponent(characterCollider, "CharacterCollider");
	
	SetupEditorCallbacks();
}

void SurvivalCharacter::Terminate()
{
	CleanupEditorCallbacks();

	if (!NetworkManager::IsServer())
	{
		CleanupCameraMovement();
		CleanupMovement();
	}

	if (characterGraphics != nullptr)
	{
		RemoveComponent("CharacterGraphics");
		delete characterGraphics;
	}

	if (characterCamera != nullptr)
	{
		RemoveComponent("CharacterCamera");
		delete characterCamera;
	}

	if (characterCollider != nullptr)
	{
		RemoveComponent("CharacterCollider");
		delete characterCollider;
	}
}

void SurvivalCharacter::GameUpdate()
{
	characterCollider->Update();

	if (!NetworkManager::IsServer())
	{
		if (SpawnedFromLocalSpawnRequest())
		{
			characterCamera->SetTarget(characterGraphics->GetPosition());

			characterCamera->SetPosition(characterCamera->GetTarget() + glm::normalize(cameraPosition) * cameraDistance);
		}

		float movementUnit = walkSpeed * TimeManager::DeltaTime();
		glm::vec3 targetVector(receivedPosition - characterGraphics->GetPosition());
		glm::vec3 direction = glm::normalize(targetVector);

		if (glm::length(targetVector) > movementUnit)
		{
			currentTranslationVector = direction * movementUnit;

			characterGraphics->Translate(currentTranslationVector);
		}
	}
	
	glm::vec3 terrainPoint = characterGraphics->GetPosition();

	Scene* scene = SceneManager::GetLoadedScene("SurvivalScene");

	if (scene != nullptr)
	{
		SurvivalTerrain* terrain = static_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));
		SurvivalWater* water = static_cast<SurvivalWater*>(scene->GetGameObject("Water"));

		if (terrain != nullptr)
		{
			TerrainComponent* terrainComponent = static_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));

			if (terrainComponent != nullptr)
			{
				terrainPoint = terrainComponent->GetTerrainPoint(characterGraphics->GetPosition());

				if (!NetworkManager::IsServer() && SpawnedFromLocalSpawnRequest())
				{
					glm::vec3 cameraTerrainPoint = terrainComponent->GetTerrainPoint(characterCamera->GetPosition());

					float cameraYRestriction = 3.0f;

					if (characterCamera->GetPosition().y < cameraTerrainPoint.y + cameraYRestriction)
					{
						characterCamera->SetPosition(cameraTerrainPoint + glm::vec3(0.0f, cameraYRestriction, 0.0f));
					}
				}
			}
		}

		if (!NetworkManager::IsServer() && SpawnedFromLocalSpawnRequest())
		{
			if (water != nullptr)
			{
				TerrainComponent* terrainComponent = static_cast<TerrainComponent*>(water->GetComponent("WaterTerrain"));

				if (terrainComponent != nullptr)
				{
					glm::vec3 cameraTerrainPoint = terrainComponent->GetTerrainPoint(characterCamera->GetPosition());

					float cameraYRestriction = 3.0f;

					if (characterCamera->GetPosition().y < cameraTerrainPoint.y + cameraYRestriction)
					{
						characterCamera->SetPosition(cameraTerrainPoint + glm::vec3(0.0f, cameraYRestriction, 0.0f));
					}
				}
			}
		}
	}

	
	characterGraphics->SetPosition(terrainPoint);

	if (NetworkManager::IsServer())
	{
		MoveToTarget();
	}
}

void SurvivalCharacter::Start()
{
	(*onEditorDisable)();

	if (!NetworkManager::IsServer() && SpawnedFromLocalSpawnRequest())
	{
		characterCamera->SetActive();
	}
}

void SurvivalCharacter::EditorUpdate()
{
	characterCollider->Update();
}

void SurvivalCharacter::Load()
{
	if (!ModelManager::ModelLoaded(CHARACTER_GRAPHICS_MODEL_NAME))
	{
		ModelManager::LoadModel(CHARACTER_GRAPHICS_MODEL_NAME, "Assets/Model/Woman.gltf");
	}

	if (!TextureManager::TextureLoaded(CHARACTER_GRAPHICS_DIFFUSE_NAME))
	{
		TextureManager::LoadTexture("Assets/Texture/Woman.png", CHARACTER_GRAPHICS_DIFFUSE_NAME);
	}
}

void SurvivalCharacter::Unload()
{
	//if (ModelManager::ModelLoaded(CHARACTER_GRAPHICS_MODEL_NAME))
	//{
	//	ModelManager::UnloadModel(CHARACTER_GRAPHICS_MODEL_NAME);
	//}
	//
	//if (TextureManager::TextureLoaded(CHARACTER_GRAPHICS_DIFFUSE_NAME))
	//{
	//	TextureManager::UnloadTexture(CHARACTER_GRAPHICS_DIFFUSE_NAME);
	//}
}

void SurvivalCharacter::Serialize()
{
	GameObject::Serialize();
}

void SurvivalCharacter::Deserialize()
{
	GameObject::Deserialize();

	characterCollider->SetGraphics(characterGraphics);
}

bool SurvivalCharacter::Hovered() const
{
	LineSegment3D lineFromScreenToWorld = Camera::CastLineFromCursorWithActiveCamera();
	return characterCollider->Intersect(lineFromScreenToWorld);
}

glm::vec3 SurvivalCharacter::GetPosition() const
{
	return characterGraphics->GetPosition();
}

glm::mat4 SurvivalCharacter::GetRotation() const
{
	return characterGraphics->GetRotation();
}

void SurvivalCharacter::SetPosition(const glm::vec3& newPosition)
{
	characterGraphics->SetPosition(newPosition);
	characterCollider->Update();
}

void SurvivalCharacter::SetRotation(const glm::mat4& newRotation)
{
	characterGraphics->SetRotation(newRotation);
	characterCollider->Update();
}

void SurvivalCharacter::SetupMovement()
{
	if (characterGraphics != nullptr)
	{
		target = characterGraphics->GetPosition();
	}

	clickToMove = new std::function<void(int)>([this](int mouseButton)
		{
			Scene* scene = SceneManager::GetLoadedScene("SurvivalScene");

			if (scene != nullptr)
			{
				SurvivalTerrain* terrain = static_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));
				SurvivalWater* water = static_cast<SurvivalWater*>(scene->GetGameObject("Water"));

				if (terrain != nullptr && water != nullptr)
				{
					TerrainComponent* terrainComponent = static_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));
					TerrainComponent* waterComponent = static_cast<TerrainComponent*>(water->GetComponent("WaterTerrain"));

					if (terrainComponent != nullptr && waterComponent != nullptr)
					{
						glm::vec3 terrainPoint = terrainComponent->GetLineIntersection(Camera::CastLineFromCursorWithActiveCamera());
						glm::vec3 waterPoint = waterComponent->GetTerrainPoint(terrainPoint);

						if (terrainPoint.y >= waterPoint.y)
						{
							target = terrainPoint;

							glm::vec3 direction = target - GetPosition();

							glm::mat4 currentRotation = characterGraphics->GetRotation();

							glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
							glm::vec4 right = glm::vec4(glm::normalize(glm::cross(glm::vec3(up), direction)), 0.0f);
							glm::vec4 forward = glm::vec4(glm::normalize(glm::cross(glm::vec3(right), glm::vec3(up))), 0.0f);

							glm::mat4 newRotation(
								right,
								up,
								forward,
								glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
							);

							characterGraphics->SetRotation(newRotation);

							ClientSend(NetworkManager::ConvertVec3ToData(target));
						}

					}
				}
			}
		});

	
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, clickToMove, "ClickToMove");
}

void SurvivalCharacter::CleanupMovement()
{
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_LEFT, "ClickToMove");

	if (clickToMove != nullptr)
	{
		delete clickToMove;
	}
}

void SurvivalCharacter::SetupCameraMovement()
{
	static glm::vec2 lastMousePos;

	cameraOrbit = new std::function<void(int)>([this](int mouseButton)
		{
			InputManager::GetCursorPosition([this](glm::vec2 pos)
				{
					float speed = 100.0f;

					float xDif = lastMousePos.x - pos.x;
					float yDif = lastMousePos.y - pos.y;

					glm::mat4 yrotation(1.0f);
					glm::vec4 vectorToRotate(glm::normalize(characterCamera->GetTarget() - characterCamera->GetPosition()), 0.0f);
					vectorToRotate = glm::normalize(vectorToRotate);
					yrotation = glm::rotate(yrotation, glm::radians(speed * TimeManager::DeltaTime() * -yDif), glm::normalize(glm::cross(glm::vec3(vectorToRotate), glm::vec3(0.0f, 1.0f, 0.0f))));

					vectorToRotate = glm::normalize(vectorToRotate * yrotation) * 20.0f;

					characterCamera->SetPosition(characterCamera->GetTarget() + glm::vec3(-vectorToRotate));

					glm::mat4 xrotation(1.0f);
					vectorToRotate = glm::vec4(glm::normalize(characterCamera->GetTarget() - characterCamera->GetPosition()), 0.0f);
					vectorToRotate = glm::normalize(vectorToRotate);
					xrotation = glm::rotate(xrotation, glm::radians(speed * TimeManager::DeltaTime() * -xDif), glm::vec3(0.0f, 1.0f, 0.0f));

					vectorToRotate = glm::normalize(vectorToRotate * xrotation) * cameraDistance;

					characterCamera->SetPosition(characterCamera->GetTarget() + glm::vec3(-vectorToRotate));

					cameraPosition = characterCamera->GetPosition() - characterCamera->GetTarget();


					lastMousePos = pos;
				});

		});

	rotateCameraViewPress = new std::function<void(int)>([](int)
		{
			InputManager::DisableCursor("Engine");

			InputManager::GetCursorPosition([](glm::vec2 pos)
				{
					lastMousePos = pos;
				});
		});

	rotateCameraViewRelease = new std::function<void(int)>([](int)
		{
			InputManager::EnableCursor("Engine");
		});

	zoomCamera = new std::function<void(double, double)>([this](double xdiff, double ydiff)
		{
			float speed = 1000.0f;

			cameraDistance += TimeManager::DeltaTime() * -(float)ydiff * speed;
		});

	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESSED, MOUSE_BUTTON_MIDDLE, cameraOrbit, "CameraOrbit");
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_3, rotateCameraViewPress, "CameraViewPress");
	InputManager::RegisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_3, rotateCameraViewRelease, "CameraViewRelease");
	InputManager::RegisterCallbackForMouseScroll(zoomCamera, "CharacterCameraZoom");

}

void SurvivalCharacter::CleanupCameraMovement()
{
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_MIDDLE, "CameraViewPress");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_MIDDLE, "CameraViewRelease");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESSED, MOUSE_BUTTON_MIDDLE, "CameraOrbit");

	if (cameraOrbit != nullptr)
	{
		delete cameraOrbit;
	}
}

void SurvivalCharacter::MoveToTarget()
{
	float movementUnit = walkSpeed * TimeManager::DeltaTime();
	glm::vec3 targetVector(target - characterGraphics->GetPosition());
	glm::vec3 direction = glm::normalize(targetVector);

	bool shouldRotate = true;

	Scene* scene = SceneManager::GetLoadedScene("SurvivalScene");

	if (scene != nullptr)
	{
		SurvivalWater* water = static_cast<SurvivalWater*>(scene->GetGameObject("Water"));
		SurvivalTerrain* terrain = static_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));

		if (water != nullptr && terrain != nullptr)
		{
			TerrainComponent* waterComp = static_cast<TerrainComponent*>(water->GetComponent("WaterTerrain"));
			TerrainComponent* terrainComp = static_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));

			if (waterComp != nullptr && terrainComp != nullptr)
			{
				glm::vec3 futureWaterTerrainPoint = waterComp->GetTerrainPoint(characterGraphics->GetPosition() + direction * movementUnit);
				glm::vec3 futureTerrainPoint = terrainComp->GetTerrainPoint(characterGraphics->GetPosition() + direction * movementUnit);

				if (futureTerrainPoint.y <= futureWaterTerrainPoint.y)
				{
					return;
				}
			}
		}

		for (auto& gameObject : scene->GetGameObjects())
		{
			SurvivalTree* tree = dynamic_cast<SurvivalTree*>(gameObject.second);

			if (tree != nullptr)
			{
				OrientedBoundingBoxComponent* treeCollider = static_cast<OrientedBoundingBoxComponent*>(tree->GetComponent("Collider"));
				if (characterCollider->Intersect(*treeCollider))
				{
					characterGraphics->Translate(-glm::normalize(treeCollider->GetOrigin() - characterGraphics->GetPosition()) * movementUnit);
					shouldRotate = false;
					//ServerSendAll(NetworkManager::ConvertVec3ToData(characterGraphics->GetPosition()));
					target = glm::vec3(0.0f);
				}
			}
		}
	}

	if (glm::length(targetVector) > movementUnit && target != glm::zero<glm::vec3>())
	{
		characterGraphics->Translate(direction * movementUnit);

		glm::vec3 newPosition = characterGraphics->GetPosition();

		if (NetworkManager::IsServer())
		{
			ServerSendAll(NetworkManager::ConvertVec3ToData(newPosition));
		}

		if (shouldRotate)
		{
			glm::mat4 currentRotation = characterGraphics->GetRotation();

			glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			glm::vec4 right = glm::vec4(glm::normalize(glm::cross(glm::vec3(up), direction)), 0.0f);
			glm::vec4 forward = glm::vec4(glm::normalize(glm::cross(glm::vec3(right), glm::vec3(up))), 0.0f);

			glm::mat4 newRotation(
				right,
				up,
				forward,
				glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
			);

			characterGraphics->SetRotation(newRotation);
		}
		
	}
}

void SurvivalCharacter::SetupEditorCallbacks()
{
	onEditorEnable = new std::function<void()>([this]()
		{
			if (!characterCollider->IsVisible())
			{
				characterCollider->ToggleVisibility();
			}
		});

	onEditorDisable = new std::function<void()>([this]()
		{
			if (characterCollider->IsVisible())
			{
				characterCollider->ToggleVisibility();
			}
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);
	Editor::RegisterOnEditorDisable(onEditorDisable);
}

void SurvivalCharacter::CleanupEditorCallbacks()
{
	if (onEditorDisable != nullptr)
	{
		Editor::DeregisterOnEditorDisable(onEditorDisable);
		delete onEditorDisable;
	}

	if (onEditorEnable != nullptr)
	{
		Editor::DeregisterOnEditorEnable(onEditorEnable);
		delete onEditorEnable;
	}

}
