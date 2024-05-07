#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <unordered_map>
#include <string>

class Component;

class GameObject
{
public:

protected:

	friend class Scene;

	GameObject();

	virtual ~GameObject();

	virtual void Initialize() = 0;

	virtual void Terminate() = 0;

	virtual void Update() = 0;

	virtual void Load() = 0;

	virtual void Unload() = 0;

	void AddComponent(Component* component, const std::string name);

	void RemoveComponent(const std::string& name);

private:

	GameObject(const GameObject*) = delete;

	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;

	GameObject& operator=(GameObject&&) = delete;

	std::unordered_map<std::string, Component*> components;

};

#endif // GAMEOBJECT_H