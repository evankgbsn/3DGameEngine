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

	return nullptr;
}

const std::unordered_map<std::string, GameObject*>& Scene::GetGameObjects() const
{
	return objects;
}

void Scene::Start()
{
	for (auto& object : objects)
	{
		object.second->Start();
	}
}

void Scene::End()
{
	for (auto& object : objects)
	{
		object.second->End();
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

void Scene::Deserialize(const std::vector<char>& sceneData)
{
}

void Scene::Save()
{
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
	for (auto& object : objects)
	{
		object.second->EditorUpdate();
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
