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
#include "GameEngine/Time/TimeManager.h"
#include "GameEngine/Collision/StaticCollider.h"

#include "GameEngine/Math/Shapes/LineSegment3D.h"


Character::Character() :
	graphics(nullptr),
	collider(nullptr),
	toggleColliderVisibility(nullptr)
{
	float moveSpeed = 1.0f;

	forward = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(0.0f, 0.0f, 1.0f) * TimeManager::DeltaTime() * moveSpeed);
		});

	backward = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(0.0f, 0.0f, -1.0f) * TimeManager::DeltaTime() * moveSpeed);
		});

	left = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(1.0f, 0.0f, 0.0f) * TimeManager::DeltaTime() * moveSpeed);
		});

	right = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(-1.0f, 0.0f, 0.0f) * TimeManager::DeltaTime() * moveSpeed);
		});

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
	treeGraphics = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Sphere"), TextureManager::GetTexture("CrateTree"), TextureManager::GetTexture("GreyTree"));
	//treeGraphics->Scale({ 2.0f, 2.0f, 2.0f });
	treeGraphics->SetTranslation({ 10.5f, 2.0f, 10.5f });

	treeCollider = new StaticCollider(treeGraphics);

	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("RandomGrey"), TextureManager::GetTexture("Random"));
	graphics->SetShine(32.0f);
	graphics->SetClip(0);
	graphics->Translate({ 0.0f, -0.5f, 0.0f });

	collider = new AnimatedCollider(graphics);

	graphics2 = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("RandomGrey"), TextureManager::GetTexture("Random"));
	graphics2->SetShine(32.0f);
	graphics2->SetClip(3);
	graphics2->Translate({ 10.0f, -0.5f, 0.0f });
	graphics2->Rotate(90.0f, { 0.0f, 1.0f, 0.0f });

	collider2 = new AnimatedCollider(graphics2);

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, toggleColliderVisibility, "CharacterColliderVisibility");
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, castLine, "CastLineFromCamera");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, forward, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, backward, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, left, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, right, "walk");

	obb = new OrientedBoundingBoxWithVisualization(ModelManager::GetModel("Woman")->GetVertices());
}

void Character::Terminate()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "CharacterColliderVisibility");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, "CastLineFromCamera");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "walk");

	delete collider;
	delete collider2;
	delete obb;

	GraphicsObjectManager::Delete(graphics);
	GraphicsObjectManager::Delete(graphics2);
}

void Character::Update()
{
	Camera& cam = CameraManager::GetActiveCamera();

	cam.SetPosition(graphics->GetTranslation() + glm::vec3(0.0f, 8.0f, -8.0f));

	cam.SetTarget(graphics->GetTranslation());

	collider->Update();
	//collider->Intersect(*obb);
	collider2->Update();

	treeCollider->Update();

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

	collider->Intersect(lineFromScreenToWorld);

	collider->Intersect(*collider2);

	collider->Intersect(*treeCollider);

}

void Character::Load()
{
	if (!ModelManager::ModelLoaded("Cube"))
	{
		ModelManager::LoadModel("Cube", "Assets/Model/Cube.gltf");
	}

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

	if (!TextureManager::TextureLoaded("RandomGrey"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "RandomGrey");
	}


	if (!ModelManager::ModelLoaded("Woman"))
	{
		ModelManager::LoadModel("Woman", "Assets/Model/Woman.gltf");
	}

	if (!ModelManager::ModelLoaded("Tree"))
	{
		ModelManager::LoadModel("Tree", "Assets/Model/Tree.gltf");
	}

	if (!TextureManager::TextureLoaded("CrateTree"))
	{
		TextureManager::LoadTexture("Assets/Texture/container2.png", "CrateTree");
	}

	if (!TextureManager::TextureLoaded("GreyTree"))
	{
		TextureManager::LoadTexture("Assets/Texture/Grey.png", "GreyTree");
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

	if (TextureManager::TextureLoaded("RandomGrey"))
	{
		TextureManager::UnloadTexture("RandomGrey");
	}

}
