#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <vector>

class Component;

class GameObject
{
public:

	virtual bool Hovered() const;

	virtual void SetPosition(const glm::vec3& pos) {};

	virtual glm::vec3 GetPosition() const { static glm::vec3 defaultReturn; return defaultReturn; };

	virtual void Start() {};

	virtual void End() {};

	virtual std::vector<char> Serialize() { return std::vector<char>(); };

	virtual void Deserialize() {};

protected:

	friend class Scene;

	GameObject();

	virtual ~GameObject();

	virtual void Initialize() = 0;

	virtual void Terminate() = 0;

	virtual void GameUpdate() = 0;

	virtual void EditorUpdate() = 0;

	virtual void Load() = 0;

	virtual void Unload() = 0;

	void AddComponent(Component* component, const std::string name);

	void RemoveComponent(const std::string& name);

	Component* const GetComponent(const std::string& component) const;

private:

	GameObject(const GameObject*) = delete;

	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;

	GameObject& operator=(GameObject&&) = delete;

	std::unordered_map<std::string, Component*> components;

};

#endif // GAMEOBJECT_H