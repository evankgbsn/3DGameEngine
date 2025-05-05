#include "GameObject.h"

const std::string& GameObject::GetName() const
{
	return name;
}

GameObject::GameObject(const std::string& n) :
	name(n)
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