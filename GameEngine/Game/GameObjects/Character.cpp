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

#include <glm/gtc/matrix_access.hpp>


Character::Character() :
	graphics(nullptr),
	collider(nullptr),
	toggleColliderVisibility(nullptr),
	targetPosition({0.0f, 0.0f, 0.0f})
{
	float moveSpeed = 3.0f;

	forward = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(0.0f, 0.0f, 1.0f) * TimeManager::DeltaTime() * moveSpeed);
			graphics->SetTranslation(terrain->GetTerrainPoint(graphics->GetTranslation()));
		});

	backward = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(0.0f, 0.0f, -1.0f) * TimeManager::DeltaTime() * moveSpeed);
			graphics->SetTranslation(terrain->GetTerrainPoint(graphics->GetTranslation()));
		});

	left = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(1.0f, 0.0f, 0.0f) * TimeManager::DeltaTime() * moveSpeed);
			graphics->SetTranslation(terrain->GetTerrainPoint(graphics->GetTranslation()));
		});

	right = new std::function<void(int)>([this, moveSpeed](int keyCode)
		{
			graphics->Translate(glm::vec3(-1.0f, 0.0f, 0.0f) * TimeManager::DeltaTime() * moveSpeed);
			graphics->SetTranslation(terrain->GetTerrainPoint(graphics->GetTranslation()));
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

			LineSegment3D lineFromScreenToWorld(cam.GetPosition(), glm::vec3(x));

			Ray rayFromScreenToWorld(cam.GetPosition(), glm::normalize(glm::vec3(x) - cam.GetPosition()));

			const std::vector<std::vector<AxisAlignedBoundingBoxWithVisualization*>>& boxes = terrain->GetCellArray();

			unsigned int count = 0;

			for (const auto& boxArray : boxes)
			{
				for (AxisAlignedBoundingBoxWithVisualization* aabb : boxArray)
				{
					if (aabb->LineIntersect(lineFromScreenToWorld))
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

	//GOColoredInstanced* inst = GraphicsObjectManager::CreateGO3DColoredInstanced(ModelManager::GetModel("Woman"), { 1.0f, 0.0f, 0.0f, 1.0f }, 1);
	//inst->SetTranslation({0.0f, 0.0f, 0.0f}, 0);
	//inst->SetRotation(glm::mat4(1.0f), 0);
	//inst->SetScale({ 1.0f, 1.0f, 1.0f }, 0);

	Text* text = new Text("This is test text", "arial");

	treeGraphics = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Cube"), TextureManager::GetTexture("RandomGrey"), TextureManager::GetTexture("RandomGrey"));
	//treeGraphics->Scale({ 2.0f, 2.0f, 2.0f });
	treeGraphics->SetTranslation({ 10.5f, 0.0f, 10.5f });
	treeGraphics->SetShine(8.0f);


	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("Woman"), TextureManager::GetTexture("Random"));
	graphics->SetShine(32.0f);
	graphics->SetClip(7);

	collider = new AnimatedCollider(graphics);

	graphics2 = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("RandomGrey"), TextureManager::GetTexture("Random"));
	graphics2->SetShine(32.0f);
	graphics2->SetClip(3);
	graphics2->Translate({ 10.0f, -0.5f, 0.0f });

	skybox = GraphicsObjectManager::Create3DGOTextured(ModelManager::LoadModel("Skybox", "Assets/Model/Skybox.gltf"), TextureManager::LoadTexture("Assets/Texture/Skybox2.png", "Skybox"));
	skybox->SetScale({ 1000.0f, 1000.0f, 1000.0f });
	

	collider2 = new AnimatedCollider(graphics2);

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, toggleColliderVisibility, "CharacterColliderVisibility");
	InputManager::RegisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, castLine, "CastLineFromCamera");
	//InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, forward, "walk");
	//InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, backward, "walk");
	//InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, left, "walk");
	//InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, right, "walk");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_H, moveCamLeft, "CameraMove");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_K, moveCamRight, "CameraMove");

	//obb = new OrientedBoundingBoxWithVisualization(ModelManager::GetModel("Woman")->GetVertices());

	cameraTarget = glm::vec3(0.0f, 0.0f, 10.0f);
	camPosition = glm::vec3(0.0f, 7.0f, -10.0f);

	terrain = new Terrain("Terrain", "Assets/Texture/Noise.png", "Grey", "Grey", 1000, 1000, 400, 400, 30, -30);

	treeGraphics->SetTranslation(terrain->GetTerrainPoint(treeGraphics->GetTranslation()));
	treeCollider = new StaticCollider(treeGraphics);
	graphics2->SetTranslation(terrain->GetTerrainPoint(graphics2->GetTranslation()));
}

void Character::Terminate()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "CharacterColliderVisibility");
	InputManager::DeregisterCallbackForMouseButtonState(KEY_PRESS, MOUSE_BUTTON_1, "CastLineFromCamera");
	//InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_W, "walk");
	//InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_S, "walk");
	//InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_A, "walk");
	//InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_D, "walk");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_H, "CameraMove");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESSED, KEY_K, "CameraMove");

	delete collider;
	delete collider2;
	//delete obb;

	GraphicsObjectManager::Delete(graphics);
	GraphicsObjectManager::Delete(graphics2);
}

void Character::GameUpdate()
{
	if (graphics->GetTranslation() != targetPosition)
	{
		graphics->Translate(glm::normalize(targetPosition - graphics->GetTranslation()) * TimeManager::DeltaTime() * 3.0f);
		graphics->SetTranslation(terrain->GetTerrainPoint(graphics->GetTranslation()));
	}

	graphics2->Rotate(5.0f * TimeManager::DeltaTime(), {0.0f, 1.0f, 0.0f});

	Camera& cam = CameraManager::GetActiveCamera();

	cam.SetPosition(graphics->GetTranslation() + camPosition);

	cam.SetTarget(graphics->GetTranslation() + cameraTarget);

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

	const glm::vec3 normal = glm::normalize(glm::vec3(x) - cam.GetPosition());

	Ray ray(cam.GetPosition(), normal);

	Plane plane(glm::vec3(0.0f, 1.0f, 0.0f), treeGraphics->GetTranslation().y);

	glm::vec3 planePoint = cam.GetPosition() + normal * plane.RayIntersect(ray);

	//if (treeCollider->Intersect(ray) != -1)
	//{

	const glm::vec3 terrainPoint = terrain->GetTerrainPoint(planePoint);
		
	const glm::vec3 translation = treeGraphics->GetTranslation() - terrainPoint;

	treeCollider->Translate(-translation);

	treeGraphics->SetTranslation(terrainPoint);
	//};

	collider->Intersect(lineFromScreenToWorld);


	//collider->Intersect(*treeCollider);

	collider->Intersect(*collider2);
}

void Character::EditorUpdate()
{
}

void Character::Load()
{
	if (!TextureManager::TextureLoaded("Green"))
	{
		TextureManager::LoadTexture("Assets/Texture/OIP.jpg", "Green");
	}

	if (!TextureManager::TextureLoaded("RockGround"))
	{
		TextureManager::LoadTexture("Assets/Texture/RockGround.png", "RockGround");
	}

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
