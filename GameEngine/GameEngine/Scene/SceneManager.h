#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include <unordered_map>

class Scene;

class SceneManager
{

public:

	static void Initialize();

	static void Terminate();

	static void RegisterScene(Scene* const scene, const std::string& name);

	static void DegegisterScene(const std::string& name);

	static void LoadScene(const std::string& sceneName);

	static void UnloadScene(const std::string& sceneName);

private:

	friend class SingletonHelpers;

	friend class Engine;

	SceneManager();

	~SceneManager();

	SceneManager(const SceneManager&) = delete;

	SceneManager& operator=(const SceneManager&) = delete;

	SceneManager(SceneManager&&) = delete;

	SceneManager& operator=(SceneManager&&) = delete;

	static void Update();

	static SceneManager* instance;

	std::unordered_map<std::string, Scene*> registeredScenes;

	std::unordered_map<std::string, Scene*> loadedScenes;
};


#endif // SCENEMANAGER_H