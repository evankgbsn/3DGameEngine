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
#include "Networking/NetworkManager.h"
#include "Database/DatabaseManager.h"
#include "Audio/AudioManager.h"


#include <GLFW/glfw3.h>
#include <mimalloc-new-delete.h>

#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"
#endif

Engine* Engine::instance = nullptr;

void Engine::Initialize()
{
	SingletonHelpers::InitializeSingleton<Engine>(&instance, "Engine");

	instance->editorState = Editor::IsEnabled();

#ifdef _SERVER
	NetworkManager::Start(true);
#endif

#ifdef _CLIENT
	NetworkManager::Start(false);
#ifdef _WIN32
	FreeConsole();
#endif
#endif

}

void Engine::Run()
{
	while (!Renderer::ShouldTerminate())
	{
		auto managePlay = []()
			{
				if (instance->editorState != Editor::IsEnabled())
				{
					if (instance->editorState)
					{
						SceneManager::StartInitializedScenes();
					}
					else
					{
						SceneManager::EndStartedScenes();
					}
				}

				instance->editorState = Editor::IsEnabled();
			};

#ifndef GAME
		if (Editor::IsEnabled())
		{
			TimeManager::RecordUpdateTime();
			ModelManager::Update();
			Renderer::Update();
			Editor::Update();
			InputManager::EditorUpdate();
			SceneManager::EditorUpdate();
			NetworkManager::EditorUpdate();
			AudioManager::EditorUpdate();
		}
		else
#endif
		{
			TimeManager::RecordUpdateTime();
			ModelManager::Update();
			PhysicsManager::Update();
			Renderer::Update();
			InputManager::Update();
			SceneManager::Update();
			NetworkManager::Update();
			AudioManager::GameUpdate();
		}

		WindowManager::SetWindowTitle("Engine", std::string("Engine ") + "FPS: " + std::to_string(TimeManager::GetAverageFPS()) + " Network Latency: " + std::to_string(NetworkManager::GetLatency()) + "ms");

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

Window* Engine::GetWindow()
{
	return WindowManager::GetWindow("Engine");
}

bool Engine::IsEditorBuild()
{
	if (instance != nullptr)
	{
		return instance->isEditorBuild;
	}

	return false;
}

Engine::Engine()
{
	Logger::ClearLog(true);
	DatabaseManager::Initialize();
	TimeManager::Initialize();
	Renderer::Initialize();
	InputManager::Initialize();

	glm::vec2 primaryMonitorDimensions = Renderer::GetPrimaryMonitorDimensions();

	Renderer::CreateMainWindow(static_cast<unsigned int>(primaryMonitorDimensions.x / 2), static_cast<unsigned int>(primaryMonitorDimensions.y / 2), "Engine");

#ifdef GAME
#ifndef _SERVER
	WindowManager::GetWindow("Engine")->ToggleFullScreen();
#endif
#endif

	SceneManager::Initialize();

#ifndef GAME
	Editor::Initialize();
	Editor::Enable();
	isEditorBuild = true;
	EditorPlayToggleInputSetup();
#endif

	PhysicsManager::Initialize();
	NetworkManager::Initialize();
	AudioManager::Initialize();
}

Engine::~Engine()
{
	AudioManager::Terminate();
	NetworkManager::Terminate();
	PhysicsManager::Terminate();
	SceneManager::Terminate();
	InputManager::Terminate();

#ifndef GAME
	Editor::Terminate();
#endif

	Renderer::Terminate();
	TimeManager::Terminate();
	DatabaseManager::Terminate();
}

void Engine::EditorPlayToggleInputSetup()
{
	static std::function<void(int)> tildaPress = std::function<void(int)>([](int keyCode)
		{
			Editor::Disable();
		});

	InputManager::EditorRegisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, &tildaPress, "Play");

	static std::function<void(int)> tildaPress2 = std::function<void(int)>([](int keyCode)
		{
			Editor::Enable();
		});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_GRAVE_ACCENT, &tildaPress2, "Play");
}
