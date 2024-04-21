#include "WindowManager.h"

#include "../../Utils/Logger.h"
#include "../../Utils/SingletonHelpers.h"
#include "../Shader/ShaderManager.h"
#include "../Model/ModelManager.h"
#include "../Texture/TextureManager.h"
#include "../GraphicsObjects/GraphicsObjectManager.h"
#include "../Light/LightManager.h"

WindowManager* WindowManager::instance = nullptr;

Window* const WindowManager::GetWindow(const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->windows.find(name) != instance->windows.end())
		{
			return instance->windows[name];
		}
		else
		{
			Logger::Log("Could not find window: " + name + " WindowManager::GetWindow()", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log("Calling WindowManager::GetWindow() before WindowManager::Initialize()", Logger::Category::Warning);
	}
	return nullptr;
}

void WindowManager::Initialize()
{
	SingletonHelpers::InitializeSingleton<WindowManager>(&instance, "WindowManager");
}

void WindowManager::Terminate()
{
	SingletonHelpers::TerminateSingleton<WindowManager>(&instance, "WindowManager");
}

void WindowManager::CreateWindow(unsigned int width, unsigned int height, const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->windows.find(name) == instance->windows.end())
		{
			instance->windows[name] = new Window(static_cast<int>(width), static_cast<int>(height), name);

			if (instance->windows.size() == 1)
			{
				instance->mainWindow = instance->windows[name];
				ShaderManager::Initialize();
				ModelManager::Initialize();
				TextureManager::Initialize();
				LightManager::Initialize();
				GraphicsObjectManager::Initialize();
			}
		}
		else
		{
			Logger::Log("A window with the name " + name + " already exists!", Logger::Category::Error);
		}
	}
	else
	{
		Logger::Log("Calling WindowManager::CreateWindow() before WindowManager::Initialize()", Logger::Category::Warning);
	}
}

void WindowManager::Update()
{
	if (instance != nullptr)
	{
		if (instance->mainWindow != nullptr)
		{
			instance->shouldTerminate.store(instance->mainWindow->ShouldClose());
		}

		for (auto& window : instance->windows)
		{
			window.second->Update();
		}
	}
}

bool WindowManager::ShouldTerminate()
{
	if (instance != nullptr)
	{
		return instance->shouldTerminate.load();
	}
	else
	{
		Logger::Log("Calling WindowManager::ShouldTerminate() before WindowManager::Initialize()");
		return false;
	}
}

WindowManager::WindowManager() :
	windows(std::unordered_map<std::string, Window*>()),
	shouldTerminate(false),
	mainWindow(nullptr)
{
}

WindowManager::~WindowManager()
{
	GraphicsObjectManager::Terminate();
	LightManager::Terminate();
	TextureManager::Terminate();
	ModelManager::Terminate();
	ShaderManager::Terminate();
	DeleteWindows();
}

void WindowManager::DeleteWindows()
{
	for (const auto& window : windows)
	{
		delete window.second;
	}
}
