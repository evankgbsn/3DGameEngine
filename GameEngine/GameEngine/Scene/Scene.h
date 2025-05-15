#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <unordered_map>

class GameObject;

class Scene
{

public:

	Scene();

	virtual ~Scene();

	GameObject* const GetGameObject(const std::string& GameObejctName) const;

	const std::unordered_map<std::string, GameObject*>& GetGameObjects() const;

	void Start();

	void End();

	void RegisterGameObject(GameObject* object, const std::string& name);

	void DeregisterGameObject(const std::string& name);
	
	void Save(const std::string& saveFileName);

	void Deserialize(const std::string& path);

protected:

	virtual void Initialize() = 0;

	virtual void Terminate() = 0;

	void Load();

	void Unload();

private:

	friend class SceneManager;

	Scene(const Scene&) = delete;

	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;

	Scene& operator=(Scene&&) = delete;

	void GameUpdate();

	void EditorUpdate();

	void InitializeObjects();

	void TerminateObjects();

	std::unordered_map<std::string, GameObject*> objects;

	bool started;

};

#endif // SCENE_H