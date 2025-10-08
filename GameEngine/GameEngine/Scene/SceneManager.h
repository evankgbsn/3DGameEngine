#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include <unordered_map>
#include <list>

class Scene;
class GameObject;

class SceneManager
{

public:

	static void Initialize();

	static void Terminate();

	static void RegisterScene(Scene* const scene, const std::string& name);

	static void DegegisterScene(const std::string& name);

	static void LoadScene(const std::string& sceneName);

	static void UnloadScene(const std::string& sceneName);

	static bool SceneLoaded(const std::string& sceneName);

	static Scene* const GetLoadedScene(const std::string& name);

	static std::vector<Scene*> GetLoadedScenes();

	static void StartLoadedScenes();

	static void EndLoadedScenes();

	static GameObject* FindGameObject(const std::string& name);

	static Scene* const GetRegisteredScene(const std::string& name);

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

	static void EditorUpdate();

	void UnloadScenes();

	static SceneManager* instance;

	std::unordered_map<std::string, Scene*> registeredScenes;

	std::unordered_map<std::string, Scene*> loadedScenes;

	std::list<std::string> scenesToUnload;
};


#endif // SCENEMANAGER_H