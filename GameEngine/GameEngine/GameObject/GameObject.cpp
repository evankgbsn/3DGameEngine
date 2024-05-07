#include "GameObject.h"

GameObject::GameObject()
{
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
	components.erase(components.find(name));
}
