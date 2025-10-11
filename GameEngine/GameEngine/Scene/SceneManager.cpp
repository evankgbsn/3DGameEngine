#include "SceneManager.h"

#include "../Utils/SingletonHelpers.h"
#include "Scene.h"

#include <list>

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
			scene->name = name;
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
			instance->registeredScenes.find(name)->second->name = "";
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
			instance->loadedScenes.insert(*instance->registeredScenes.find(sceneName));
			instance->registeredScenes.find(sceneName)->second->Load();
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
		instance->scenesToUnload.push_back(sceneName);
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

std::vector<std::string> SceneManager::GetLoadedSceneNames()
{
	std::vector<std::string> scenes;

	if (instance != nullptr)
	{
		scenes.reserve(instance->loadedScenes.size());
		for (const std::pair<const std::string&, Scene*>& scene : instance->loadedScenes)
		{
			scenes.push_back(scene.first);
		}
	}

	return scenes;
}

void SceneManager::StartLoadedScenes()
{
	if (instance != nullptr)
	{
		std::list<std::string> keys;

		for (const std::pair<const std::string&, Scene*>& scene : instance->loadedScenes)
		{
			keys.push_back(scene.first);
		}

		for (const std::string& scene : keys)
		{
			const auto& loadedScene = instance->loadedScenes.find(scene);

			if (loadedScene != instance->loadedScenes.end())
			{
				loadedScene->second->Start();
			}
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

GameObject* SceneManager::FindGameObject(const std::string& name)
{
	for (auto& scene : GetLoadedScenes())
	{
		for (auto& object : scene->objects)
		{
			if (object.first == name)
			{
				return object.second;
			}
		}
	}

	return nullptr;
}

Scene* const SceneManager::GetRegisteredScene(const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(name) != instance->registeredScenes.end())
		{
			return instance->registeredScenes[name];
		}
	}

	return nullptr;
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
		instance->UnloadScenes();

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
		instance->UnloadScenes();

		for (auto& scene : instance->loadedScenes)
		{
			scene.second->EditorUpdate();
		}
	}
}

void SceneManager::UnloadScenes()
{
	for (const std::string& sceneName : scenesToUnload)
	{
		const auto& sceneToUnload = loadedScenes.find(sceneName);

		if (sceneToUnload != loadedScenes.end())
		{
			sceneToUnload->second->Unload();
			loadedScenes.erase(sceneToUnload);
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been loaded. SceneManager::UnloadScene()", Logger::Category::Warning);
		}
	}

	scenesToUnload.clear();
}
