#include "GameObject.h"

#include "Component/Component.h"

#include <typeinfo>

std::unordered_map<std::string, std::function<void(GameObject**)>> GameObject::newFunctions = std::unordered_map<std::string, std::function<void(GameObject**)>>();

void GameObject::Serialize()
{
	for (auto& comp : components)
	{
		comp.second->Serialize();
	}
}

void GameObject::Deserialize()
{
	std::vector<Component*> bodies;

	for (auto& comp : components)
	{
		if (comp.second->nameOfType == "RigidBodyComponent")
		{
			bodies.push_back(comp.second);
		}
		else
		{
			comp.second->Deserialize();
		}
	}

	for (auto& comp : bodies)
	{
		comp->Deserialize();
	}
}

const std::string& GameObject::GetName() const
{
	return name;
}

void GameObject::SetName(const std::string& n)
{
	name = n;
}

GameObject::GameObject(const std::string& n) :
	name(n),
	owningScene(nullptr)
{
	static unsigned int IDGenerator = 0U;

	ID = IDGenerator++;
}

GameObject::~GameObject()
{
	
}

void GameObject::AddComponent(Component* component, const std::string name)
{
	components[name] = component;
}

void GameObject::RemoveComponent(const std::string& name)
{
	if (components.find(name) != components.end())
	{
		components.erase(components.find(name));
	}
}

Component* const GameObject::GetComponent(const std::string& component) const
{
	auto componentIt = components.find(component);
	if (componentIt != components.end())
	{
		return componentIt->second;
	}

	return nullptr;
}

Scene* GameObject::GetOwningScene() const
{
	return owningScene;
}

unsigned int GameObject::GetID() const
{
	return ID;
}

const std::string& GameObject::GetNameOfType() const
{
	return nameOfType;
}

bool GameObject::HasComponent(const std::string& name) const
{
	auto componentIt = components.find(name);
	if (componentIt != components.end())
	{
		return true;
	}

	return false;
}

std::function<void(GameObject**)> GameObject::GetConstructor(const std::string& name)
{
	if (newFunctions.find(name) != newFunctions.end())
	{
		return newFunctions[name];
	}

	return std::function<void(GameObject**)>();
}

bool GameObject::Hovered() const
{
	return false;
}