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
#include "Renderer/Texture/TextureManager.h";
#include "Input/InputManager.h"
#include "Renderer/Camera/CameraManager.h"
#include "Renderer/Camera/Camera.h"
#include "Collision/AnimatedCollider.h"
#include "Renderer/Light/LightManager.h"

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
		instance->character->Rotate(0.01f, { 0.0f, 1.0f, 0.0f });

		instance->collider->Update();
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

	TextureManager::LoadTexture("./Assets/Texture/container2.png", "Crate");
	
	LightManager::CreateDirectionalLight({2.0f, 2.0f, 2.0f, 2.0f}, {0.5f, 0.5f, 0.5f, 0.0f});
	LightManager::SetAmbientIntensity(0.2f);
	LightManager::CreatePointLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { 3.0f, 4.0f, 1.5f });

	box = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel("Cube"), TextureManager::GetTexture("Crate"));

	TextureManager::LoadTexture("./Assets/Texture/grey.png", "Grey");

	character = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel("Woman"), TextureManager::GetTexture("Woman"));

	character->SetClip(3);

	collider = new AnimatedCollider(character);

	float cameraSpeed = 0.01f;
	wPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * cameraSpeed);
		});

	aPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * -cameraSpeed);
		});
	sPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetForwardVector() * -cameraSpeed);
		});

	dPress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(cam.GetRightVector() * cameraSpeed);
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
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * -cameraSpeed);
		});

	spacePress = new std::function<void(int)>([cameraSpeed](int keycode)
		{
			Camera& cam = CameraManager::GetActiveCamera();
			cam.Translate(glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed);
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
