#ifndef COMPONENT_H
#define COMPONENT_H

#include "../../Utils/Logger.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class Component
{

public:

	virtual void Serialize() = 0;

	virtual void Deserialize() = 0;

	static std::function<void(Component**)> GetConstructor(const std::string& name);

protected:

	Component();

	~Component();

	template<typename T>
	void RegisterComponentClassType(Component* obj);

	std::unordered_map<std::string, int> savedInts;

	std::unordered_map<std::string, float> savedFloats;

	std::unordered_map<std::string, glm::vec3> savedVec3s;

	std::unordered_map<std::string, glm::vec2> savedVec2s;

	std::unordered_map<std::string, glm::vec4> savedVec4s;

	std::unordered_map<std::string, glm::mat4> savedMat4s;

	std::unordered_map<std::string, std::string> savedStrings;

	std::unordered_map<std::string, bool> savedBools;

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
	nameOfType.replace(0, 6, "");
	Logger::Log(std::string("Created Component of Type: ") + nameOfType, Logger::Category::Info);

	newFunctions[nameOfType] = std::function<void(Component**)>([](Component** outNewGameObject)
		{
			*outNewGameObject = new T();
		});

}

#endif // COMPONENT_H