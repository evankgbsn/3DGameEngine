#include "SurvivalCharacter.h"


#include "GameEngine/GameObject/Component/GraphicsObjectTexturedAnimatedLit.h"
#include "GameEngine/GameObject/Component/GraphicsObjectTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/GameObject/Component/CameraComponent.h"
#include "GameEngine/GameObject/Component/AnimatedColliderComponent.h"
#include "GameEngine/GameObject/Component/OrientedBoundingBoxComponent.h"
#include "GameEngine/GameObject/Component/StaticColliderComponent.h"
#include "GameEngine/Math/Shapes/LineSegment3D.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "SurvivalTerrain.h"
#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/GameObject/Component/WaterComponent.h"
#include "GameEngine/Renderer/Camera/Camera.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Collision/AxisAlignedBoundingBoxWithVisualization.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Editor/Editor.h"
#include "SurvivalWater.h"
#include "SurvivalTree.h"
#include "SurvivalCrate.h"
#include "SurvivalRockLarge.h"
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

	Scene* scene = SceneManager::GetRegisteredScene("SurvivalScene");

	if (scene != nullptr)
	{
		scene->RegisterGameObject(this, "SurvivalCharacter:" + std::to_string(GetNetworkObjectID()));
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

		NetworkManager::RegisterOnClientDisconnectFunction("SurvivalCharacter:" + std::to_string(GetNetworkObjectID()), onClientDisconnect);
	}
	else
	{
		ClientSend(NetworkManager::ConvertVec3ToData(target));
	}
}

void SurvivalCharacter::OnDespawn()
{
	if (NetworkManager::IsServer())
	{
		NetworkManager::DeregisterOnClientDisconnectFunction("SurvivalCharacter:" + std::to_string(GetNetworkObjectID()));
	}

	NetworkObject::OnDespawn();
}

void SurvivalCharacter::OnDataReceived(const std::string& data)
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
		if (updateType == "Target")
		{
			target = NetworkManager::ConvertDataToVec3(updateData);
		}
		else if (updateType == "Position")
		{
			SetPosition(NetworkManager::ConvertDataToVec3(updateData));
		}
	}
	else
	{
		if (updateType == "Target")
		{
			target = NetworkManager::ConvertDataToVec3(updateData);
			ServerSendAll("Target " + NetworkManager::ConvertVec3ToData(target));
		}
	}
}

void SurvivalCharacter::OnServerSpawnConfirmation(const std::string& IP)
{
	NetworkObject::OnServerSpawnConfirmation(IP);
	ServerSend(IP, NetworkManager::ConvertVec3ToData(GetPosition()));
}

void SurvivalCharacter::OnClientSpawnConfirmation()
{
}

void SurvivalCharacter::Initialize()
{
	characterGraphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::GetModel(CHARACTER_GRAPHICS_MODEL_NAME), TextureManager::GetTexture(CHARACTER_GRAPHICS_DIFFUSE_NAME), TextureManager::GetTexture(CHARACTER_GRAPHCIS_SPECULAR_NAME));
	AddComponent(characterGraphics, "CharacterGraphics");
	
	characterGraphics->SetShine(32.0f);
	characterGraphics->SetClip(0);
	characterGraphics->SetSpeed(1.0f);
	characterGraphics->SetPosition({ 0.0f, 0.0f, 0.0f });

	characterCollider = new AnimatedColliderComponent(characterGraphics);
	AddComponent(characterCollider, "CharacterCollider");

	axe = new GraphicsObjectTexturedLit(ModelManager::GetModel("Axe"), TextureManager::GetTexture("Axe"), TextureManager::GetTexture("Axe"));
	axe->SetShine(32.0f);
	//axe->SetTransform(characterGraphics->GetJointTransform("RightHand"));
	AddComponent(axe, "Axe");


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
	
	
	
	SetupEditorCallbacks();
}

void SurvivalCharacter::Terminate()
{
	CleanupEditorCallbacks();

	if (!NetworkManager::IsServer() && SpawnedFromLocalSpawnRequest())
	{
		CleanupCameraMovement();
		CleanupMovement();
		NetworkManager::Despawn(GetNetworkObjectID());
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

	if (axe != nullptr)
	{
		RemoveComponent("Axe");
		delete axe;
	}
}

void SurvivalCharacter::GameUpdate()
{
	//axe->SetTransform(characterGraphics->GetJointTransform("RightHand"));

	characterCollider->Update();

	if (!NetworkManager::IsServer())
	{
		if (SpawnedFromLocalSpawnRequest())
		{
			characterCamera->SetTarget(characterGraphics->GetPosition());

			characterCamera->SetPosition(characterCamera->GetTarget() + glm::normalize(cameraPosition) * cameraDistance);
		}
	}
	
	glm::vec3 terrainPoint = characterGraphics->GetPosition();

	Scene* scene = SceneManager::GetRegisteredScene("SurvivalScene");

	if (scene != nullptr)
	{
		SurvivalTerrain* terrain = dynamic_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));
		SurvivalWater* water = dynamic_cast<SurvivalWater*>(scene->GetGameObject("Water"));

		if (terrain != nullptr)
		{
			TerrainComponent* terrainComponent = dynamic_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));

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
				WaterComponent* terrainComponent = dynamic_cast<WaterComponent*>(water->GetComponent("WaterTerrain"));

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

	MoveToTarget();
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
		ModelManager::LoadModel(CHARACTER_GRAPHICS_MODEL_NAME, "Assets/Model/Orc.gltf", false);
	}

	if (!ModelManager::ModelLoaded("Axe"))
	{
		ModelManager::LoadModel("Axe", "Assets/Model/Axe.gltf", false);
	}

	if (!TextureManager::TextureLoaded("Axe"))
	{
		TextureManager::LoadTexture("Assets/Texture/grey.png", "Axe");
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
			Scene* scene = SceneManager::GetRegisteredScene("SurvivalScene");

			if (scene != nullptr)
			{
				SurvivalTerrain* terrain = dynamic_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));
				SurvivalWater* water = dynamic_cast<SurvivalWater*>(scene->GetGameObject("Water"));

				if (terrain != nullptr && water != nullptr)
				{
					TerrainComponent* terrainComponent = dynamic_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));
					WaterComponent* waterComponent = dynamic_cast<WaterComponent*>(water->GetComponent("WaterTerrain"));

					if (terrainComponent != nullptr && waterComponent != nullptr)
					{
						glm::vec3 terrainPoint = terrainComponent->GetLineIntersection(Camera::CastLineFromCursorWithActiveCamera());
						glm::vec3 waterPoint = waterComponent->GetTerrainPoint(terrainPoint);

						if (terrainPoint.y >= waterPoint.y)
						{
							target = terrainPoint;

							ClientSend("Target " + NetworkManager::ConvertVec3ToData(target));
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

	Scene* scene = SceneManager::GetRegisteredScene("SurvivalScene");

	if (scene != nullptr)
	{
		SurvivalWater* water = dynamic_cast<SurvivalWater*>(scene->GetGameObject("Water"));
		SurvivalTerrain* terrain = dynamic_cast<SurvivalTerrain*>(scene->GetGameObject("Terrain"));

		if (water != nullptr && terrain != nullptr)
		{
			WaterComponent* waterComp = dynamic_cast<WaterComponent*>(water->GetComponent("WaterTerrain"));
			TerrainComponent* terrainComp = dynamic_cast<TerrainComponent*>(terrain->GetComponent("SurvivalTerrain"));

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

		if (NetworkManager::IsServer())
		{
			for (auto& gameObject : scene->GetGameObjects())
			{
				SurvivalTree* tree = dynamic_cast<SurvivalTree*>(gameObject.second);
				SurvivalCrate* crate = dynamic_cast<SurvivalCrate*>(gameObject.second);
				SurvivalRockLarge* rock = dynamic_cast<SurvivalRockLarge*>(gameObject.second);

				if (tree != nullptr)
				{
					StaticColliderComponent* treeCollider = dynamic_cast<StaticColliderComponent*>(tree->GetComponent("Collider"));
					if (characterCollider->BoxIntersect(*treeCollider))
					{
						glm::vec3 hit;
						characterCollider->Intersect(*treeCollider, hit);

						characterGraphics->Translate(-glm::normalize(tree->GetPosition() - characterGraphics->GetPosition()) * movementUnit);
						shouldRotate = false;
						ServerSendAll("Position " + NetworkManager::ConvertVec3ToData(characterGraphics->GetPosition()));
						target = glm::vec3(0.0f);
						ServerSendAll("Target " + NetworkManager::ConvertVec3ToData(target));
					}
				}
				else if (crate != nullptr)
				{
					StaticColliderComponent* crateCollider = dynamic_cast<StaticColliderComponent*>(crate->GetComponent("Collider"));
					if (characterCollider->BoxIntersect(*crateCollider))
					{
						characterGraphics->Translate(-glm::normalize(crate->GetPosition() - characterGraphics->GetPosition()) * movementUnit);
						shouldRotate = false;
						ServerSendAll("Position " + NetworkManager::ConvertVec3ToData(characterGraphics->GetPosition()));
						target = glm::vec3(0.0f);
						ServerSendAll("Target " + NetworkManager::ConvertVec3ToData(target));
					}
				}
				else if (rock != nullptr)
				{
					StaticColliderComponent* rockCollider = dynamic_cast<StaticColliderComponent*>(rock->GetComponent("Collider"));
					if (characterCollider->BoxIntersect(*rockCollider))
					{
						characterGraphics->Translate(-glm::normalize(rock->GetPosition() - characterGraphics->GetPosition()) * movementUnit);
						shouldRotate = false;
						ServerSendAll("Position " + NetworkManager::ConvertVec3ToData(characterGraphics->GetPosition()));
						target = glm::vec3(0.0f);
						ServerSendAll("Target " + NetworkManager::ConvertVec3ToData(target));
					}
				}
			}
		}
	}

	if (glm::length(targetVector) > movementUnit && target != glm::zero<glm::vec3>())
	{
		characterGraphics->Translate(direction * movementUnit);

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

	if (NetworkManager::IsServer())
	{
		static float lastPositionSend = TimeManager::SecondsSinceStart();

		if (TimeManager::SecondsSinceStart() - lastPositionSend > 0.5f)
		{
			ServerSendAll("Position " + NetworkManager::ConvertVec3ToData(characterGraphics->GetPosition()));
			lastPositionSend = TimeManager::SecondsSinceStart();
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
