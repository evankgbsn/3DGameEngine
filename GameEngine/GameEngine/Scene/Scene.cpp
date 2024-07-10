#include "Scene.h"

#include "../GameObject/GameObject.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

GameObject* const Scene::GetGameObject(const std::string& name) const
{
	if (objects.find(name) != objects.end())
	{
		return objects.find(name)->second;
	}
}

void Scene::RegisterGameObject(GameObject* object, const std::string& name)
{
	if (objects.find(name) == objects.end())
	{
		objects[name] = object;
	}
}

void Scene::DeregisterGameObject(const std::string& name)
{
	if (objects.find(name) != objects.end())
	{
		objects.erase(objects.find(name));
	}
}

void Scene::Load()
{
	Initialize();
	InitializeObjects();
}

void Scene::Unload()
{
	TerminateObjects();
	Terminate();
}

void Scene::GameUpdate()
{
	for (auto& object : objects)
	{
		object.second->GameUpdate();
	}
}

void Scene::EditorUpdate()
{
}

void Scene::InitializeObjects()
{
	for (auto& object : objects)
	{
		object.second->Load();
		object.second->Initialize();
	}
}

void Scene::TerminateObjects()
{
	for (auto& object : objects)
	{
		object.second->Terminate();
		object.second->Unload();
	}
}
