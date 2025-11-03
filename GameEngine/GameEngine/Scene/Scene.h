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

	virtual void Start();

	virtual void End();

	void RegisterGameObject(GameObject* object, const std::string& name);

	void DeregisterGameObject(const std::string& name);
	
	void Save(const std::string& saveFileName);

	void Deserialize(const std::string& path);

	const std::string& GetName() const;

	bool Loaded() const;

	bool Started() const;

	bool Initialized() const;

	void AddToCreatedObjects(const std::string& name, GameObject* obj);

	void RemoveFromCreatedObjects(const std::string& name);

	void ClearCreatedObjects();

	bool IsDeserializing() const;

protected:

	virtual void Initialize();

	virtual void Terminate();

	virtual void Load();

	virtual void Unload();

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

	void LoadObjects();

	void UnloadObjects();

	std::unordered_map<std::string, GameObject*> objects;

	bool started;

	bool initialized;

	bool loaded;

	std::string name;

	// For cleaning up objects created from deserialization or in editor.
	std::unordered_map<std::string, GameObject*> createdObjects;

	bool deserializing;

};

#endif // SCENE_H