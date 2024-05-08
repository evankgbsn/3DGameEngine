#include "Character.h"

#include "GameEngine/Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedAnimatedLit.h"
#include "GameEngine/Renderer/GraphicsObjects/GOTexturedLit.h"
#include "GameEngine/Renderer/Model/ModelManager.h"
#include "GameEngine/Renderer/Model/Model.h"
#include "GameEngine/Renderer/Texture/TextureManager.h"
#include "GameEngine/Collision/AnimatedCollider.h"
#include "GameEngine/Collision/OrientedBoundingBoxWithVisualization.h"
#include "GameEngine/Input/InputManager.h"
#include "GameEngine/Scene/SceneManager.h"
#include "GameEngine/Renderer/Window/WindowManager.h"
#include "GameEngine/Renderer/Camera/CameraManager.h"
#include "GameEngine/Renderer/Camera/Camera.h"

#include "GameEngine/Math/Shapes/LineSegment3D.h"


Character::Character() :
	graphics(nullptr),
	collider(nullptr),
	toggleColliderVisibility(nullptr)
{
	toggleColliderVisibility = new std::function<void(int)>([this](int keyCode)
		{
			collider->ToggleVisibility();
		});


	castLine = new std::function<void(int)>([this](int keyCode)
		{
			// Screen space to world space for object picking.
			Window* window = WindowManager::GetWindow("Engine");
			Camera& cam = CameraManager::GetActiveCamera();
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

			GraphicsObjectManager::CreateGOLineColored(cam.GetPosition(), x, {0.0f, 0.0f, 1.0f, 1.0f});
			LineSegment3D lineFromScreenToWorld(cam.GetPosition(), x);
		});
}

Character::~Character()
{
	delete toggleColliderVisibility;
	delete castLine;
}

void Character::Initialize()
{
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Random"));
	graphics->SetShine(32.0f);
	graphics->SetClip(7);
	graphics->Translate({ 0.0f, -0.5f, 0.0f });

	collider = new AnimatedCollider(graphics);

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, toggleColliderVisibility, "CharacterColliderVisibility");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_B, castLine, "CastLineFromCamera");

	obb = new OrientedBoundingBoxWithVisualization(ModelManager::GetModel("Woman")->GetVertices());
}

void Character::Terminate()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "CharacterColliderVisibility");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_B, "CastLineFromCamera");

	delete collider;
	delete obb;

	GraphicsObjectManager::Delete(graphics);
}

void Character::Update()
{
	collider->Update();
	collider->Intersect(*obb);

	

}

void Character::Load()
{
	if (!ModelManager::ModelLoaded("Home"))
	{
		ModelManager::LoadModel("Home", "Assets/Model/HomeModel.gltf");
	}

	if (!TextureManager::TextureLoaded("Woman"))
	{
		TextureManager::LoadTexture("Assets/Texture/Woman.png", "Woman");
	}

	if (!TextureManager::TextureLoaded("Random"))
	{
		TextureManager::LoadTexture("Assets/Texture/container2_specular.png", "Random");
	}


	if (!ModelManager::ModelLoaded("Woman"))
	{
		ModelManager::LoadModel("Woman", "Assets/Model/Woman.gltf");
	}
}

void Character::Unload()
{
	if (ModelManager::ModelLoaded("Home"))
	{
		ModelManager::UnloadModel("Home");
	}

	if (TextureManager::TextureLoaded("Woman"))
	{
		TextureManager::UnloadTexture("Woman");
	}

	if (ModelManager::ModelLoaded("Woman"))
	{
		ModelManager::UnloadModel("Woman");
	}
}
