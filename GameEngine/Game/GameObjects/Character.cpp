#include "Character.h"

#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedAnimatedLit.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedLit.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTextured.h"
#include "GameEngine/Renderer/GraphicsObjects/GOColoredInstanced.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Collision/AnimatedCollider.h"
#include "GameEngine/Collision/OrientedBoundingBoxWithVisualization.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "GameEngine/Renderer/Window/WindowManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Renderer/Camera/Camera.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Collision/StaticCollider.h"
#include "GameEngine/Renderer/Text/Text.h"
#include "GameEngine/Math/Shapes/LineSegment3D.h"
#include "GameEngine/Terrain/Terrain.h"
#include "GameEngine/Collision/AxisAlignedBoundingBoxWithVisualization.h"
#include "GameEngine/Math/Shapes/Ray.h"
#include "GameEngine/Utils/Logger.h"
#include "GameEngine/Math/Shapes/Plane.h"
#include "GameTerrain.h"

#include <glm/gtc/matrix_access.hpp>


Character::Character() :
	graphics(nullptr),
	collider(nullptr),
	toggleColliderVisibility(nullptr),
	targetPosition({0.0f, 0.0f, 0.0f})
{
	float moveSpeed = 3.0f;

	Terrain* terrain = GetTerrain();

	auto snapToTerrain = [this, terrain]()
		{
			targetPosition = graphics->GetTranslation();
		};

	forward = new std::function<void(int)>([this, moveSpeed, snapToTerrain](int keyCode)
		{
			graphics->Translate(glm::vec3(0.0f, 0.0f, 1.0f) * TimeManager::DeltaTime() * moveSpeed);
			snapToTerrain();
			
		});

	backward = new std::function<void(int)>([this, moveSpeed, snapToTerrain](int keyCode)
		{
			graphics->Translate(glm::vec3(0.0f, 0.0f, -1.0f) * TimeManager::DeltaTime() * moveSpeed);
			snapToTerrain();
		});

	left = new std::function<void(int)>([this, moveSpeed, snapToTerrain](int keyCode)
		{
			graphics->Translate(glm::vec3(1.0f, 0.0f, 0.0f) * TimeManager::DeltaTime() * moveSpeed);
			snapToTerrain();
		});

	right = new std::function<void(int)>([this, moveSpeed, snapToTerrain](int keyCode)
		{
			graphics->Translate(glm::vec3(-1.0f, 0.0f, 0.0f) * TimeManager::DeltaTime() * moveSpeed);
			snapToTerrain();
		});

	toggleColliderVisibility = new std::function<void(int)>([this](int keyCode)
		{
			collider->ToggleVisibility();
		});


	updateCollisionVisuals = new std::function<void(int)>([this](int keycode)
		{
			Terrain* terrain = GetTerrain();

			terrain->UpdateTerrainCells();
		});

	castLine = new std::function<void(int)>([this](int keyCode)
		{
			Terrain* terrain = GetTerrain();

			LineSegment3D lineToWorld = Camera::CastLineFromCursorWithActiveCamera();

			if (terrain != nullptr)
			{
				const std::vector<std::vector<AxisAlignedBoundingBoxWithVisualization*>>& boxes = terrain->GetCellArray();

				unsigned int count = 0;

				for (const auto& boxArray : boxes)
				{
					for (AxisAlignedBoundingBoxWithVisualization* aabb : boxArray)
					{
						if (aabb->LineIntersect(lineToWorld))
						{
							targetPosition = terrain->GetTerrainPoint(aabb->GetOrigin());

							glm::vec3 translation = graphics->GetTranslation();

							glm::vec3 forward = glm::normalize(glm::vec3(graphics->GetRotation()[2]));
							glm::vec3 toTarget = -glm::normalize(glm::normalize(glm::vec3(translation.x, 0.0f, translation.z) - glm::vec3(targetPosition.x, 0.0f, targetPosition.z)));
							glm::vec3 right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), toTarget);

							glm::mat4 rotation(
								glm::vec4(glm::normalize(right), 0.0f),
								glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
								glm::vec4(toTarget, 0.0f),
								graphics->GetRotation()[3]
							);

							graphics->SetRotation(rotation);

							count++;
						}
					}
				}

				count++;
			}
			
		});

	float cameraMoveSpeed = 2.0f;

	moveCamLeft = new std::function<void(int)>([cameraMoveSpeed, this](int)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			
			glm::vec3 target = cam.GetTarget();

			cameraTarget += glm::vec3(cameraMoveSpeed * TimeManager::DeltaTime(), 0.0f, 0.0f);

			camPosition += glm::vec3(cameraMoveSpeed * TimeManager::DeltaTime(), 0.0f, 0.0f);

		});

	moveCamRight = new std::function<void(int)>([cameraMoveSpeed, this](int)
		{
			Camera& cam = CameraManager::GetActiveCamera();

			glm::vec3 target = cam.GetTarget();

			cameraTarget += glm::vec3(-cameraMoveSpeed * TimeManager::DeltaTime(), 0.0f, 0.0f);

			camPosition += glm::vec3(-cameraMoveSpeed * TimeManager::DeltaTime(), 0.0f, 0.0f);

		});
}

Character::~Character()
{
	delete toggleColliderVisibility;
	delete castLine;
}

void Character::Initialize()
{
	Text* text = new Text("This is test text", "arial");

	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("Woman"), TextureManager::GetTexture("Random"));
	graphics->SetShine(32.0f);
	graphics->SetClip(7);
	collider = new AnimatedCollider(graphics);

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, toggleColliderVisibility, "CharacterColliderVisibility");
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, castLine, "CastLineFromCamera");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, forward, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, backward, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, left, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, right, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_H, moveCamLeft, "CameraMove");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_K, moveCamRight, "CameraMove");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_B, updateCollisionVisuals, "ColisionVisuals");

	cameraTarget = glm::vec3(0.0f, 0.0f, 10.0f);
	camPosition = glm::vec3(0.0f, 7.0f, -10.0f);
}

void Character::Terminate()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "CharacterColliderVisibility");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, "CastLineFromCamera");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_H, "CameraMove");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_K, "CameraMove");

	delete collider;

	GraphicsObjectManager::Delete(graphics);
}

void Character::GameUpdate()
{
	Terrain* terrain = GetTerrain();

	if (graphics->GetTranslation() != targetPosition)
	{
		graphics->Translate(glm::normalize(targetPosition - graphics->GetTranslation()) * TimeManager::DeltaTime() * 3.0f);
		terrain->UpdateHeightByTerrainPoint(graphics->GetTranslation(), TimeManager::DeltaTime() * -1.0f);
	}

	if (terrain != nullptr)
		graphics->SetTranslation(terrain->GetTerrainPoint(graphics->GetTranslation()));

	Camera& cam = CameraManager::GetCamera("Main");

	cam.SetPosition(graphics->GetTranslation() + camPosition);

	cam.SetTarget(graphics->GetTranslation() + cameraTarget);

	collider->Update();
}

void Character::EditorUpdate()
{
	collider->Update();
}

void Character::Load()
{
	if (!TextureManager::TextureLoaded("Woman"))
	{
		TextureManager::LoadTexture("Assets/Texture/Woman.png", "Woman");
	}

	if (!ModelManager::ModelLoaded("Woman"))
	{
		ModelManager::LoadModel("Woman", "Assets/Model/Woman.gltf");
	}

	if (!TextureManager::TextureLoaded("Random"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "Random");
	}
}

void Character::Unload()
{

}

bool Character::Hovered() const
{
	Camera& cam = CameraManager::GetActiveCamera();

	// Screen space to world space for object picking.
	Window* window = WindowManager::GetWindow("Engine");
	glm::vec2 cursorPos = window->GetCursorPosition();
	glm::mat4 invPersp = glm::inverse(cam.GetProjection());
	glm::mat4 invView = glm::inverse(cam.GetView());
	glm::mat4 screenToNDC(
		glm::vec4((float)window->GetWidth() / 2.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0, -(float)window->GetHeight() / 2.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		glm::vec4((float)window->GetWidth() / 2.0f, (float)window->GetHeight() / 2.0f, 0.5f, 1.0f)
	);
	screenToNDC = glm::inverse(screenToNDC);

	glm::vec4 x = glm::vec4(cursorPos.x, cursorPos.y, 1.0f, 1.0f);
	x = screenToNDC * x;
	x = invPersp * x;
	x = invView * x;
	x /= x.w;

	LineSegment3D lineFromScreenToWorld(cam.GetPosition(), x);
	return collider->Intersect(lineFromScreenToWorld);
}

void Character::SetPosition(const glm::vec3& newPos)
{
	graphics->SetTranslation(newPos);
	collider->Update();
}

void Character::Start()
{
	if (graphics != nullptr)
	{
		targetPosition = graphics->GetTranslation();
	}
}

glm::vec3 Character::GetPosition()
{
	return graphics->GetTranslation();
}

Terrain* Character::GetTerrain() const
{
	Scene* mainScene = SceneManager::GetLoadedScene("Main");

	Terrain* terrain = nullptr;

	if (mainScene != nullptr)
	{
		GameTerrain* gameTerrainObject = static_cast<GameTerrain*>(mainScene->GetGameObject("Terrain"));

		if (gameTerrainObject != nullptr)
		{
			terrain = gameTerrainObject->GetTerrain();
		}
	}

	return terrain;
}
