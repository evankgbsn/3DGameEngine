#include "SceneManager.h"

#include "../Utils/SingletonHelpers.h"
#include "Scene.h"

SceneManager* SceneManager::instance = nullptr;

void SceneManager::Initialize()
{
	SingletonHelpers::InitializeSingleton(&instance, "SceneManager");
}

void SceneManager::Terminate()
{
	SingletonHelpers::TerminateSingleton(&instance, "SceneManager");
}

void SceneManager::RegisterScene(Scene* const scene, const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(name) == instance->registeredScenes.end())
		{
			instance->registeredScenes[name] = scene;
		}
		else
		{
			Logger::Log("A scene with the name " + name + " has already been registered.", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log("Calling SceneManager::RegisterScene() before SceneManager::Initialize()", Logger::Category::Error);
	}
}

void SceneManager::DegegisterScene(const std::string& name)
{
	if(instance != nullptr)
	{
		if (instance->registeredScenes.find(name) != instance->registeredScenes.end())
		{
			instance->registeredScenes.erase(instance->registeredScenes.find(name));
		}
		else
		{
			Logger::Log("A scene with the name " + name + " has never been registered.", Logger::Category::Warning);
		}
	}
	else
	{
		Logger::Log("Calling SceneManager::DeregisterScene() before SceneManager::Initialize()", Logger::Category::Error);
	}
}

void SceneManager::LoadScene(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			instance->registeredScenes.find(sceneName)->second->Load();
			instance->loadedScenes.insert(*instance->registeredScenes.find(sceneName));
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::LoadScene()", Logger::Category::Warning);
		}
	}
}

void SceneManager::UnloadScene(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->loadedScenes.find(sceneName) != instance->loadedScenes.end())
		{
			instance->loadedScenes[sceneName]->Unload();
			instance->loadedScenes.erase(instance->loadedScenes.find(sceneName));
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been loaded. SceneManager::UnloadScene()", Logger::Category::Warning);
		}
	}
}

bool SceneManager::SceneLoaded(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		return instance->loadedScenes.find(sceneName) != instance->loadedScenes.end();
	}
	else
	{
		Logger::Log("Calling SceneManager::SceneLoaded() before SceneManager::Initialize()", Logger::Category::Warning);
	}

	return false;
}

Scene* const SceneManager::GetLoadedScene(const std::string& name)
{
	if (instance != nullptr)
	{
		const auto& query = instance->loadedScenes.find(name);
		if (query != instance->loadedScenes.end())
		{
			return query->second;
		}
	}
	else
	{
		Logger::Log("Calling SceneManager::GetScene() before SceneManager::Initialize()", Logger::Category::Error);

		return nullptr;
	}

	Logger::Log("The scene" + name + "has not been loaded.SceneManager::GetLoadedScene", Logger::Category::Warning);

	return nullptr;
}

std::vector<Scene*> SceneManager::GetLoadedScenes()
{
	std::vector<Scene*> scenes;

	if (instance != nullptr)
	{
		scenes.reserve(instance->loadedScenes.size());
		for (const std::pair<const std::string&, Scene*>& scene : instance->loadedScenes)
		{
			scenes.push_back(scene.second);
		}
	}

	return scenes;
}

void SceneManager::StartLoadedScenes()
{
	if (instance != nullptr)
	{
		for (const std::pair<const std::string&, Scene*>& scene : instance->loadedScenes)
		{
			scene.second->Start();
		}
	}
}

void SceneManager::EndLoadedScenes()
{
	if (instance != nullptr)
	{
		for (const std::pair<const std::string&, Scene*>& scene : instance->loadedScenes)
		{
			scene.second->End();
		}
	}
}

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	if (instance != nullptr)
	{
		for (auto& scene : instance->loadedScenes)
		{
			scene.second->GameUpdate();
		}
	}
}

void SceneManager::EditorUpdate()
{
	if (instance != nullptr)
	{
		for (auto& scene : instance->loadedScenes)
		{
			scene.second->EditorUpdate();
		}
	}
}
