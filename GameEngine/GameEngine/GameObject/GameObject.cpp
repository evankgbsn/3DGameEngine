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

Component* const GameObject::GetComponent(const std::string& component) const
{
	return nullptr;
}

bool GameObject::Hovered() const
{
	return false;
}