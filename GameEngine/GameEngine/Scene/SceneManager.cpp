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
			if (!instance->registeredScenes.find(sceneName)->second->Loaded())
			{
				instance->registeredScenes.find(sceneName)->second->Load();
			}
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::LoadScene()", Logger::Category::Warning);
		}
	}
}

void SceneManager::InitializeScene(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			if (!instance->registeredScenes.find(sceneName)->second->Initialized() && instance->registeredScenes.find(sceneName)->second->Loaded())
			{
				instance->registeredScenes.find(sceneName)->second->Initialize();
			}
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::InitializeScene()", Logger::Category::Warning);
		}
	}
}

void SceneManager::TerminateScene(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			if (instance->registeredScenes.find(sceneName)->second->Initialized() && instance->registeredScenes.find(sceneName)->second->Loaded() && !instance->registeredScenes.find(sceneName)->second->Started())
			{
				instance->registeredScenes.find(sceneName)->second->Terminate();
			}
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::TerminateScene()", Logger::Category::Warning);
		}
	}
}

void SceneManager::StartScene(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			if (instance->registeredScenes.find(sceneName)->second->Initialized() && instance->registeredScenes.find(sceneName)->second->Loaded() && !instance->registeredScenes.find(sceneName)->second->Started())
			{
				instance->registeredScenes.find(sceneName)->second->Start();
			}
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::StartScene()", Logger::Category::Warning);
		}
	}
}

void SceneManager::EndScene(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			if (instance->registeredScenes.find(sceneName)->second->Initialized() && instance->registeredScenes.find(sceneName)->second->Loaded() && instance->registeredScenes.find(sceneName)->second->Started())
			{
				instance->registeredScenes.find(sceneName)->second->End();
			}
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::EndScene()", Logger::Category::Warning);
		}
	}
}

void SceneManager::UnloadScene(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			if (instance->registeredScenes.find(sceneName)->second->Loaded() && !instance->registeredScenes.find(sceneName)->second->Started())
			{
				instance->scenesToUnload.push_back(sceneName);
			}
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::UnloadScene()", Logger::Category::Warning);
		}
	}
}

bool SceneManager::SceneLoaded(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			return instance->registeredScenes.find(sceneName)->second->Loaded();
		}
	}
	else
	{
		Logger::Log("Calling SceneManager::SceneLoaded() before SceneManager::Initialize()", Logger::Category::Warning);
	}

	return false;
}

bool SceneManager::SceneInitialized(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			return instance->registeredScenes.find(sceneName)->second->Initialized();
		}
	}
	else
	{
		Logger::Log("Calling SceneManager::SceneInitialized() before SceneManager::Initialize()", Logger::Category::Warning);
	}

	return false;
}

bool SceneManager::SceneStarted(const std::string& sceneName)
{
	if (instance != nullptr)
	{
		if (instance->registeredScenes.find(sceneName) != instance->registeredScenes.end())
		{
			return instance->registeredScenes.find(sceneName)->second->Started();
		}
	}
	else
	{
		Logger::Log("Calling SceneManager::SceneStarted() before SceneManager::Initialize()", Logger::Category::Warning);
	}

	return false;
}

Scene* const SceneManager::GetRegisteredScene(const std::string& name)
{
	if (instance != nullptr)
	{
		const auto& query = instance->registeredScenes.find(name);
		if (query != instance->registeredScenes.end())
		{
			return query->second;
		}
	}
	else
	{
		Logger::Log("Calling SceneManager::GetRegisteredScene() before SceneManager::Initialize()", Logger::Category::Error);

		return nullptr;
	}

	Logger::Log("The scene" + name + "has not been loaded.SceneManager::GetLoadedScene", Logger::Category::Warning);

	return nullptr;
}

std::vector<Scene*> SceneManager::GetRegisteredScenes()
{
	std::vector<Scene*> scenes;

	if (instance != nullptr)
	{
		scenes.reserve(instance->registeredScenes.size());
		for (const std::pair<const std::string&, Scene*>& scene : instance->registeredScenes)
		{
			scenes.push_back(scene.second);
		}
	}

	return scenes;
}

std::vector<std::string> SceneManager::GetRegisteredSceneNames()
{
	std::vector<std::string> scenes;

	if (instance != nullptr)
	{
		scenes.reserve(instance->registeredScenes.size());
		for (const std::pair<const std::string&, Scene*>& scene : instance->registeredScenes)
		{
			scenes.push_back(scene.first);
		}
	}

	return scenes;
}

void SceneManager::StartInitializedScenes()
{
	if (instance != nullptr)
	{
		std::list<std::string> keys;

		for (const std::pair<const std::string&, Scene*>& scene : instance->registeredScenes)
		{
			keys.push_back(scene.first);
		}

		for (const std::string& scene : keys)
		{
			const auto& initializedScene = instance->registeredScenes.find(scene);

			if (initializedScene != instance->registeredScenes.end())
			{
				if (initializedScene->second->Initialized())
				{
					initializedScene->second->Start();
				}
			}
		}
	}
}

void SceneManager::EndStartedScenes()
{
	if (instance != nullptr)
	{
		for (const std::pair<const std::string&, Scene*>& scene : instance->registeredScenes)
		{
			if (scene.second->Started())
			{
				scene.second->End();
			}
		}
	}
}

void SceneManager::InitializeLoadedScenes()
{
	if (instance != nullptr)
	{
		std::list<std::string> keys;

		for (const std::pair<const std::string&, Scene*>& scene : instance->registeredScenes)
		{
			keys.push_back(scene.first);
		}

		for (const std::string& scene : keys)
		{
			const auto& loadedScene = instance->registeredScenes.find(scene);

			if (loadedScene != instance->registeredScenes.end())
			{
				if (loadedScene->second->Loaded())
				{
					loadedScene->second->Initialize();
				}
			}
		}
	}
}

void SceneManager::TerminateEndedScenes()
{
	if (instance != nullptr)
	{
		for (const std::pair<const std::string&, Scene*>& scene : instance->registeredScenes)
		{
			if (!scene.second->Started() && scene.second->Initialized())
			{
				scene.second->Terminate();
			}
		}
	}
}

void SceneManager::UnloadTerminatedScenes()
{
	if (instance != nullptr)
	{
		for (const std::pair<const std::string&, Scene*>& scene : instance->registeredScenes)
		{
			if (!scene.second->Started() && !scene.second->Initialized() && scene.second->Loaded())
			{
				scene.second->Unload();
			}
		}
	}
}

GameObject* SceneManager::FindGameObject(const std::string& name)
{
	for (auto& scene : GetRegisteredScenes())
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

		for (auto& scene : instance->registeredScenes)
		{
			if (scene.second->Started())
			{
				scene.second->GameUpdate();
			}
		}
	}
}

void SceneManager::EditorUpdate()
{
	if (instance != nullptr)
	{
		instance->UnloadScenes();

		for (auto& scene : instance->registeredScenes)
		{
			if (scene.second->Initialized())
			{
				scene.second->EditorUpdate();
			}
		}
	}
}

void SceneManager::UnloadScenes()
{
	for (const std::string& sceneName : scenesToUnload)
	{
		const auto& sceneToUnload = registeredScenes.find(sceneName);

		if (sceneToUnload != registeredScenes.end())
		{
			if (sceneToUnload->second->Loaded())
			{
				sceneToUnload->second->Unload();
			}
			else
			{
				Logger::Log("A scene with the name " + sceneName + " has not been loaded. SceneManager::UnloadScene()", Logger::Category::Warning);
			}
		}
		else
		{
			Logger::Log("A scene with the name " + sceneName + " has not been registered. SceneManager::UnloadScene()", Logger::Category::Warning);
		}
	}

	scenesToUnload.clear();
}
