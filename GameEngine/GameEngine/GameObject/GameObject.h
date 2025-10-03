#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "../Utils/Logger.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

class Component;

class GameObject
{
public:

	virtual bool Hovered() const;

	virtual void SetPosition(const glm::vec3& pos) {};

	virtual glm::vec3 GetPosition() const { static glm::vec3 defaultReturn; return defaultReturn; };

	virtual void SetRotation(const glm::mat4& rotation) {};

	virtual glm::mat4 GetRotation() const { static glm::mat4 defaultReturn; return defaultReturn; };

	virtual glm::mat4 GetTransform() const { static glm::mat4 defaultReturn; return defaultReturn; };

	virtual void Start() {};

	virtual void End() {};

	virtual void Serialize();

	virtual void Deserialize();

	virtual const std::string& GetName() const;

	void SetName(const std::string& name);

	Component* const GetComponent(const std::string& component) const;

protected:

	friend class Scene;

	GameObject() = delete;

	GameObject(const std::string& name);

	virtual ~GameObject();

	virtual void Initialize() = 0;

	virtual void Terminate() = 0;

	virtual void GameUpdate() = 0;

	virtual void EditorUpdate() = 0;

	virtual void Load() = 0;

	virtual void Unload() = 0;

	template<typename T>
	void RegisterGameObjectClassType(GameObject* obj);

	void AddComponent(Component* component, const std::string name);

	void RemoveComponent(const std::string& name);

	static std::function<void(GameObject**)> GetConstructor(const std::string& name);

private:

	GameObject(const GameObject*) = delete;

	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;

	GameObject& operator=(GameObject&&) = delete;

	static std::unordered_map<std::string, std::function<void(GameObject**)>> newFunctions;

	std::string name;

	std::string nameOfType;

	std::unordered_map<std::string, Component*> components;

};

template<typename T>
inline void GameObject::RegisterGameObjectClassType(GameObject* obj)
{
	nameOfType = std::string(typeid(*obj).name());
	nameOfType.replace(0, 6, "");
	Logger::Log(std::string("Created GameObject of Type: ") + nameOfType, Logger::Category::Info);

	newFunctions[nameOfType] = std::function<void(GameObject**)>([](GameObject** outNewGameObject)
		{
			*outNewGameObject = new T();
		});

}

#endif // GAMEOBJECT_H