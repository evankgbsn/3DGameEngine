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

	static void InitializeScene(const std::string& sceneName);

	static void TerminateScene(const std::string& sceneName);

	static void StartScene(const std::string& sceneName);

	static void EndScene(const std::string& sceneName);

	static void UnloadScene(const std::string& sceneName);

	static bool SceneLoaded(const std::string& sceneName);

	static bool SceneInitialized(const std::string& sceneName);

	static bool SceneStarted(const std::string& sceneName);

	static std::vector<Scene*> GetRegisteredScenes();

	static std::vector<std::string> GetRegisteredSceneNames();

	static void StartInitializedScenes();

	static void EndStartedScenes();

	static void InitializeLoadedScenes();

	static void TerminateEndedScenes();

	static void UnloadTerminatedScenes();

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

	std::list<std::string> scenesToUnload;
};


#endif // SCENEMANAGER_H