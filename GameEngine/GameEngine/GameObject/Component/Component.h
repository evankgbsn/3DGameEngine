#ifndef COMPONENT_H
#define COMPONENT_H

#include "../../Utils/Logger.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class Component
{

public:

	virtual const std::vector<char> Serialize() const = 0;

	virtual void Deserialize(const std::vector<char>& data) = 0;

protected:

	Component();

	~Component();

	template<typename T>
	void RegisterComponentClassType(Component* obj);

private:
	friend class Scene;

	friend class GameObject;

	Component(const Component&) = delete;

	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;

	Component& operator=(Component&&) = delete;

	virtual void Update() = 0;

	static std::unordered_map<std::string, std::function<void(Component**)>> newFunctions;

	std::string nameOfType;

};

template<typename T>
inline void Component::RegisterComponentClassType(Component* obj)
{
	nameOfType = std::string(typeid(*obj).name());
	Logger::Log(std::string("Created Component of Type: ") + nameOfType, Logger::Category::Info);

	newFunctions[nameOfType] = std::function<void(Component**)>([](Component** outNewGameObject)
		{
			*outNewGameObject = new T();
		});

}

#endif // COMPONENT_H