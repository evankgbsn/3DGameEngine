#include "Character.h"

#include "GameEngine/GameObject/Component/GraphicsObjectTexturedAnimatedLit.h"
#include "GameEngine/GameObject/Component/AnimatedColliderComponent.h"
#include "GameEngine/GameObject/Component/TextComponent.h"
#include "GameEngine/GameObject/Component/TerrainComponent.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Scene/Scene.h"
#include "GameEngine/Renderer/Window/WindowManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Renderer/Camera/Camera.h"
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Renderer/Text/Text.h"
#include "GameEngine/Math/Shapes/LineSegment3D.h"
#include "GameEngine/Terrain/Terrain.h"
#include "GameEngine/Math/Shapes/Ray.h"
#include "GameEngine/Collision/AxisAlignedBoundingBoxWithVisualization.h"
#include "GameEngine/Utils/Logger.h"
#include "GameEngine/Math/Shapes/Plane.h"
#include "GameTerrain.h"
#include "GameEngine/UI/Button.h"
#include "GameEngine/Editor/Editor.h"
#include "GameTerrain.h"

#include <glm/gtc/matrix_access.hpp>


Character::Character() : 
	GameObject("Character"),
	graphics(nullptr),
	collider(nullptr),
	toggleColliderVisibility(nullptr),
	targetPosition({0.0f, 0.0f, 0.0f}),
	cameraDistance(20.0f)
{
	float moveSpeed = 3.0f;

	TerrainComponent* terrain = GetTerrain();

	auto snapToTerrain = [this]()
		{
			targetPosition = graphics->GetPosition();
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

	castLine = new std::function<void(int)>([this](int keyCode)
		{
			TerrainComponent* terrain = GetTerrain();

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

							glm::vec3 translation = graphics->GetPosition();

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

	static glm::vec2 lastMousePos;

	rotateCameraView = new std::function<void(int)>([this](int)
		{
			InputManager::GetCursorPosition([this](glm::vec2 pos) 
				{
					float speed = 100.0f;

					float xDif = lastMousePos.x - pos.x;
					float yDif = lastMousePos.y - pos.y;

					Camera& cam = CameraManager::GetActiveCamera();

					//cam.Translate({0.0f, yDif * TimeManager::DeltaTime() * speed, 0.0f });

					glm::mat4 yrotation(1.0f);
					glm::vec4 vectorToRotate(glm::normalize(cam.GetTarget() - cam.GetPosition()), 0.0f);
					vectorToRotate = glm::normalize(vectorToRotate);
					yrotation = glm::rotate(yrotation, glm::radians(speed * TimeManager::DeltaTime() * -yDif), glm::normalize(glm::cross(glm::vec3(vectorToRotate), glm::vec3(0.0f, 1.0f, 0.0f))));

					vectorToRotate = glm::normalize(vectorToRotate * yrotation) * 20.0f;

					cam.SetPosition(cam.GetTarget() + glm::vec3(-vectorToRotate));

					glm::mat4 xrotation(1.0f);
					vectorToRotate = glm::vec4(glm::normalize(cam.GetTarget() - cam.GetPosition()), 0.0f);
					vectorToRotate = glm::normalize(vectorToRotate);
					xrotation = glm::rotate(xrotation, glm::radians(speed * TimeManager::DeltaTime() * -xDif), glm::vec3(0.0f, 1.0f, 0.0f));

					vectorToRotate = glm::normalize(vectorToRotate * xrotation) * cameraDistance;

					cam.SetPosition(cam.GetTarget() + glm::vec3(-vectorToRotate));

					camPosition =  cam.GetPosition() - cam.GetTarget();


					lastMousePos = pos;
				});

		});

	rotateCameraViewPress = new std::function<void(int)>([](int)
		{
			WindowManager::GetWindow("Engine")->DisableCursor();

			InputManager::GetCursorPosition([](glm::vec2 pos)
				{
					lastMousePos = pos;
				});
		});

	rotateCameraViewRelease = new std::function<void(int)>([](int)
		{
			WindowManager::GetWindow("Engine")->EnableCursor();
		});

	zoomCamera = new std::function<void(double, double)>([this](double xdiff, double ydiff)
		{
			float speed = 1000.0f;

			cameraDistance += TimeManager::DeltaTime() * -(float)ydiff * speed;
		});
}

Character::~Character()
{
	delete forward;
	delete backward;
	delete left;
	delete right;
	delete toggleColliderVisibility;
	delete castLine;
	delete updateCollisionVisuals;
	delete moveCamLeft;
	delete moveCamRight;
	delete rotateCameraView;
	delete rotateCameraViewPress;
	delete rotateCameraViewRelease;
	delete zoomCamera;
}

void Character::Initialize()
{
	text = new TextComponent("This is test text gggg jjjj", "arial");

	AddComponent(text, "TestText");

	graphics = new GraphicsObjectTexturedAnimatedLit(ModelManager::GetModel("Soldier"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Random"));
	graphics->SetShine(32.0f);
	graphics->SetClip(3);
	graphics->SetSpeed(1.0f);

	AddComponent(graphics, "CharacterGraphics");

	collider = new AnimatedColliderComponent(graphics);

	AddComponent(collider, "CharacterGraphicsCollider");

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, toggleColliderVisibility, "CharacterColliderVisibility");
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, castLine, "CastLineFromCamera");
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESSED, MOUSE_BUTTON_3, rotateCameraView, "Camera");
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_3, rotateCameraViewPress, "Camera");
	InputManager::RegisterCallbackForMouseButtonState(KEY_RELEASE, MOUSE_BUTTON_3, rotateCameraViewRelease, "Camera");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, forward, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, backward, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, left, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, right, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_H, moveCamLeft, "CameraMove");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_K, moveCamRight, "CameraMove");
	InputManager::RegisterCallbackForMouseScroll(zoomCamera, "CharacterCameraZoom");

	cameraTarget = glm::vec3(0.0f, 0.0f, 10.0f);
	camPosition = glm::vec3(0.0f, 7.0f, -10.0f);

	CameraManager::CreateCamera(Camera::Type::PERSPECTIVE, "Main", WindowManager::GetWindow("Engine"));

	Camera& cam = CameraManager::GetCamera("Main");

	cam.SetPosition(graphics->GetPosition() + camPosition);
}

void Character::Terminate()
{
	CameraManager::DestroyCamera("Main");

	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "CharacterColliderVisibility");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, "CastLineFromCamera");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESSED, MOUSE_BUTTON_3, "Camera");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_H, "CameraMove");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_K, "CameraMove");

	delete collider;

	delete graphics;

	delete text;
}

void Character::GameUpdate()
{
	Camera& cam = CameraManager::GetCamera("Main");

	cam.SetTarget(graphics->GetPosition());
	
	cam.SetPosition(cam.GetTarget() + glm::normalize(camPosition) * cameraDistance);

	TerrainComponent* terrain = GetTerrain();

	if (graphics->GetPosition() != targetPosition)
	{
		graphics->Translate(glm::normalize(targetPosition - graphics->GetPosition()) * TimeManager::DeltaTime() * 5.0f);
		//terrain->UpdateHeightByTerrainPoint(graphics->GetTranslation(), TimeManager::DeltaTime() * -1.0f);
	}

	if (terrain != nullptr)
		graphics->SetPosition(terrain->GetTerrainPoint(graphics->GetPosition()));



	collider->Update();

	glm::vec2 texPos = text->GetPosition();

	float speed = 4.0f;

	glm::vec2 newTextPos = texPos + glm::vec2(speed* TimeManager::DeltaTime(), speed* TimeManager::DeltaTime());

	text->SetPosition(newTextPos);


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

	if (!ModelManager::ModelLoaded("Soldier"))
	{
		ModelManager::LoadModel("Soldier", "Assets/Model/Soldier.gltf");
	}

	if (!TextureManager::TextureLoaded("Random"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "Random");
	}

	if (!TextureManager::TextureLoaded("Soldier"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "Soldier");
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
	graphics->SetPosition(newPos);
	collider->Update();
}

void Character::SetRotation(const glm::mat4& newRotation)
{
	graphics->SetRotation(newRotation);
	collider->Update();
}

void Character::Start()
{
	if (graphics != nullptr)
	{
		targetPosition = graphics->GetPosition();
	}

	if (collider->IsVisible() && !Editor::IsEnabled())
	{
		collider->ToggleVisibility();
	}
}

void Character::End()
{
	if (!collider->IsVisible())
	{
		collider->ToggleVisibility();
	}
}

glm::vec3 Character::GetPosition() const
{
	return graphics->GetPosition();
}

glm::mat4 Character::GetRotation() const
{
	return graphics->GetRotation();
}

TerrainComponent* Character::GetTerrain() const
{
	Scene* mainScene = SceneManager::GetLoadedScene("Main");

	TerrainComponent* terrain = nullptr;

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

const std::vector<char> Character::Serialize() const
{
	return std::vector<char>();
}

void Character::Deserialize(const std::vector<char>& data)
{
}
