#include "Scene.h"

#include "../GameObject/GameObject.h"

Scene::Scene()
{
}

Scene::~Scene()
{
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

void Scene::Update()
{
	for (auto& object : objects)
	{
		object.second->Update();
	}
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
