#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <unordered_map>

class GameObject;

class Scene
{

public:

	Scene();

	~Scene();

protected:

	virtual void Initialize() = 0;

	virtual void Terminate() = 0;

	void RegisterGameObject(GameObject* object, const std::string& name);

	void DeregisterGameObject(const std::string& name);

private:

	friend class SceneManager;

	Scene(const Scene&) = delete;

	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;

	Scene& operator=(Scene&&) = delete;
	
	void Load();

	void Unload();

	void GameUpdate();

	void EditorUpdate();

	void InitializeObjects();

	void TerminateObjects();

	std::unordered_map<std::string, GameObject*> objects;

};

#endif // SCENE_H