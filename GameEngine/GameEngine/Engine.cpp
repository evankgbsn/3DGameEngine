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
#include "Renderer/Light/DirectionalLight.h"
#include "Collision/OrientedBoundingBoxWithVisualization.h"
#include "Renderer/Model/Model.h"
#include "Scene/SceneManager.h"
#include "Editor/Editor.h"
#include "Physics/PhysicsManager.h"

#include <GLFW/glfw3.h>

Engine* Engine::instance = nullptr;

void Engine::Initialize()
{
	SingletonHelpers::InitializeSingleton<Engine>(&instance, "Engine");

	instance->editorState = Editor::Enabled();
}

void Engine::Run()
{
	while (!Renderer::ShouldTerminate())
	{
		auto managePlay = []()
			{
				if (instance->editorState != Editor::Enabled())
				{
					if (instance->editorState)
					{
						SceneManager::StartLoadedScenes();
					}
					else
					{
						SceneManager::EndLoadedScenes();
					}
				}

				instance->editorState = Editor::Enabled();
			};


		if (Editor::Enabled())
		{
			TimeManager::RecordUpdateTime();
			Renderer::Update();
			InputManager::EditorUpdate();
			SceneManager::EditorUpdate();
			Editor::Update();
		}
		else
		{
			TimeManager::RecordUpdateTime();
			Renderer::Update();
			InputManager::Update();
			SceneManager::Update();
		}
		

		managePlay();
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
	SceneManager::Initialize();
	Editor::Initialize();
	Editor::Enable();
	EditorPlayToggleInputSetup();
	PhysicsManager::Initialize();
}

Engine::~Engine()
{
	PhysicsManager::Terminate();
	Editor::Terminate();
	SceneManager::Terminate();
	InputManager::Terminate();
	Renderer::Terminate();
	TimeManager::Terminate();
}

void Engine::EditorPlayToggleInputSetup()
{
	static std::function<void(int)> tildaPress = std::function<void(int)>([](int keyCode)
		{
			Editor::Disbale();
		});

	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, &tildaPress, "Play");

	static std::function<void(int)> tildaPress2 = std::function<void(int)>([](int keyCode)
		{
			Editor::Enable();
		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, &tildaPress2, "Play");
}
