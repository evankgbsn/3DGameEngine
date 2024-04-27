#include "Engine.h"

#include "Utils/SingletonHelpers.h"
#include "Renderer/Renderer.h"
#include "Time/TimeManager.h"
#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/GOTexturedAnimated.h"
#include "Renderer/GraphicsObjects/GOTexturedAnimatedLit.h"
#include "Renderer/GraphicsObjects/GOTextured.h"
#include "Renderer/GraphicsObjects/GOTexturedLit.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Texture/TextureManager.h"
#include "Input/InputManager.h"
#include "Renderer/Camera/CameraManager.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Window/WindowManager.h"
#include "Renderer/Window/Window.h"
#include "Collision/AnimatedCollider.h"
#include "Renderer/Light/LightManager.h"
#include "Collision/OrientedBoundingBoxWithVisualization.h"
#include "Renderer/Model/Model.h"

#include <GLFW/glfw3.h>

Engine* Engine::instance = nullptr;

void Engine::Initialize()
{
	SingletonHelpers::InitializeSingleton<Engine>(&instance, "Engine");
}

void Engine::Run()
{
	while (!Renderer::ShouldTerminate())
	{
		TimeManager::RecordUpdateTime();
		Renderer::Update();
		InputManager::Update();

		instance->box->Rotate(0.01f, { 0.0f, 1.0f, 0.0f });
		instance->box->Rotate(0.01f, { 1.0f, 0.0f, 0.0f });
		static OrientedBoundingBoxWithVisualization* obb = new OrientedBoundingBoxWithVisualization(ModelManager::GetModel("Woman")->GetVertices());
		instance->collider->Update();
		instance->collider->Intersect(*obb);

		double xpos, ypos;
		glfwGetCursorPos(WindowManager::GetWindow("Engine")->GetGLFWwindow(), &xpos, &ypos);
		static double prevx, prevy;

		Camera& cam = CameraManager::GetActiveCamera();

		float xspeed = 0.0005f;
		float yspeed = 0.0005f;
		if (xpos != prevx)
		{
			cam.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), xspeed * (float)-(xpos - prevx));
		}
		if (ypos != prevy)
		{
			cam.Rotate(cam.GetRightVector(), yspeed * (float)-(ypos - prevy));
		}

		prevx = xpos;
		prevy = ypos;
	}
}

void Engine::Terminate()
{
	SingletonHelpers::TerminateSingleton<Engine>(&instance, "Engine");
}

WindowManager* Engine::GetWindowManager()
{
	return nullptr;
}

Engine::Engine()
{

	TimeManager::Initialize();
	Renderer::Initialize();
	InputManager::Initialize();
	Renderer::CreateMainWindow(1920U, 1080U, "Engine");
	glfwSetInputMode(WindowManager::GetWindow("Engine")->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	TextureManager::LoadTexture("./Assets/Texture/grey.png", "Grey");
	TextureManager::LoadTexture("./Assets/Texture/container2.png", "Crate");
	TextureManager::LoadTexture("./Assets/Texture/container2_specular.png", "CrateSpecular");
	
	LightManager::CreateDirectionalLight({1.0f, 1.0f, 1.0f, 1.0f}, {-0.5f, -0.5f, 0.5f, 0.0f});
	LightManager::SetAmbientIntensity(0.02f);
	LightManager::CreatePointLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { -1.0f, 4.0f, 4.5f });


	float count = 10;
	float dist = 5.0f;
	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < count; ++j)
		{
			//LightManager::CreateSpotLight({ 5.0f, 5.0f, 5.0f, 0.0f }, { dist * i, 5.0f, dist * j }, { 0.0f, -1.0f, 0.3f });
		}
	}

	for (unsigned int i = 4; i < count; ++i)
	{
		for (unsigned int j = 4; j < count; ++j)
		{
			box = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Cube"), TextureManager::GetTexture("Crate"), TextureManager::GetTexture("CrateSpecular"));
			box->SetShine(32.0f);
			box->SetTranslation({ dist * i, 0.5f, dist * j });
		}
	}

	ModelManager::LoadModel("Worrior", "Assets/Model/Worrior.gltf");
	ModelManager::LoadModel("Biker", "Assets/Model/Biker.gltf");

	character = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Biker"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("CrateSpecular"));
	character->SetShine(32.0f);
	character->SetClip(0);
	character->Translate({ 0.0f, -0.5f, 0.0f });

	GOTexturedLit* largePlane = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("LargePlane"), TextureManager::GetTexture("Grey"), TextureManager::GetTexture("Grey"));
	largePlane->Translate({ 0.0f, -0.5f, 0.0f });
	largePlane->SetShine(32.0f);

	GOTexturedLit* tree = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Tree"), TextureManager::GetTexture("Crate"), TextureManager::GetTexture("Grey"));
	tree->Scale({ 2.0f, 2.0f, 2.0f });

	tree->SetTranslation({ 10.5f, -1.0f, 10.5f });

	collider = new AnimatedCollider(character);

	float cameraSpeed = 5.0f;
	wPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	aPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * -cameraSpeed * TimeManager::DeltaTime());
		});
	sPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * -cameraSpeed * TimeManager::DeltaTime());
		});

	dPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * cameraSpeed * TimeManager::DeltaTime());
		});

	float rotSpeed = 0.001f;
	qPress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetRightVector(), rotSpeed);
		});
	ePress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetRightVector(), -rotSpeed);
		});

	zPress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetUpVector(), rotSpeed);
		});

	cPress = new std::function<void(int)>([rotSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Rotate(cam.GetUpVector(), -rotSpeed);
		});

	ctrPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSpeed * TimeManager::DeltaTime());
		});

	spacePress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed * TimeManager::DeltaTime());
		});
	
	iPress = new std::function<void(int)>([](int keycode)
		{
			instance->character->SetDrawMode(GO3D::Mode::LINE);
			//GraphicsObjectManager::Disable(go);
		});

	jPress = new std::function<void(int)>([](int keycode)
		{
			//GraphicsObjectManager::Enable(go);
			instance->character->SetDrawMode(GO3D::Mode::FILL);
		});

	escPress = new std::function<void(int)>([](int keycode)
		{
			static bool toggle = false;
			if (toggle)
			{
				glfwSetInputMode(WindowManager::GetWindow("Engine")->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else
			{
				glfwSetInputMode(WindowManager::GetWindow("Engine")->GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			toggle = !toggle;
		});


	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, aPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, sPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, dPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Q, qPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_E, ePress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Z, zPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_C, cPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_LEFT_CTRL, ctrPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_SPACE, spacePress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_I, iPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_J, jPress, "camMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_ESCAPE, escPress, "hiddenCursor");
}

Engine::~Engine()
{
	delete collider;
	delete ctrPress;
	delete spacePress;
	delete ePress;
	delete qPress;
	delete wPress;
	delete aPress;
	delete sPress;
	delete dPress;

	InputManager::Terminate();
	Renderer::Terminate();
	TimeManager::Terminate();
}
