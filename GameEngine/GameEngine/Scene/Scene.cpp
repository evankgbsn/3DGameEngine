#include "Scene.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/Component/Component.h"

#include <filesystem>
#include <fstream>

#include <rapidxml-1.13/rapidxml.hpp>

namespace rapidxml {
	namespace internal {
		template <class OutIt, class Ch>
		inline OutIt print_children(OutIt out, const xml_node<Ch>* node, int flags, int indent);

		template <class OutIt, class Ch>
		inline OutIt print_attributes(OutIt out, const xml_node<Ch>* node, int flags);

		template <class OutIt, class Ch>
		inline OutIt print_data_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

		template <class OutIt, class Ch>
		inline OutIt print_cdata_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

		template <class OutIt, class Ch>
		inline OutIt print_element_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

		template <class OutIt, class Ch>
		inline OutIt print_declaration_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

		template <class OutIt, class Ch>
		inline OutIt print_comment_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

		template <class OutIt, class Ch>
		inline OutIt print_doctype_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

		template <class OutIt, class Ch>
		inline OutIt print_pi_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
	}
}

#include <rapidxml-1.13/rapidxml_utils.hpp>
#include <rapidxml-1.13/rapidxml_iterators.hpp>
#include <rapidxml-1.13/rapidxml_print.hpp>


Scene::Scene() :
	started(false),
	savedName("Scene")
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
	started = true;
}

void Scene::End()
{
	for (auto& object : objects)
	{
		object.second->End();
	}
	started = false;
}

void Scene::RegisterGameObject(GameObject* object, const std::string& name)
{
	if (objects.find(name) == objects.end())
	{
		objects[name] = object;

		if (started)
		{
			object->Initialize();
			object->Start();
		}
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
	// Create a document.
	rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
	rapidxml::xml_node<>* decl = doc->allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc->allocate_attribute("version", "1.0"));
	decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
	doc->append_node(decl);

	// Make the scene root node.
	rapidxml::xml_node<>* scene = doc->allocate_node(rapidxml::node_element, "Scene");
	rapidxml::xml_attribute<>* name = doc->allocate_attribute("type", typeid(*this).name());
	rapidxml::xml_attribute<>* type = doc->allocate_attribute("name", savedName.c_str());
	scene->append_attribute(type);
	scene->append_attribute(name);
	doc->append_node(scene);

	for (auto& obj : GetGameObjects())
	{
		rapidxml::xml_node<>* object = doc->allocate_node(rapidxml::node_element, "Object");
		rapidxml::xml_attribute<>* objectType = doc->allocate_attribute("type", obj.second->nameOfType.c_str());
		rapidxml::xml_attribute<>* objectName = doc->allocate_attribute("name", obj.first.c_str());
		object->append_attribute(objectType);
		object->append_attribute(objectName);

		for (auto& comp : obj.second->components)
		{
			rapidxml::xml_node<>* component = doc->allocate_node(rapidxml::node_element, "Component");
			rapidxml::xml_attribute<>* componentType = doc->allocate_attribute("type", comp.second->nameOfType.c_str());
			rapidxml::xml_attribute<>* componentName = doc->allocate_attribute("name", comp.first.c_str());
			component->append_attribute(componentType);
			component->append_attribute(componentName);

			object->append_node(component);
		}

		scene->append_node(object);
	}

	// Convert the document to a string.
	std::string document;
	rapidxml::print(std::back_inserter(document), *doc);
	//char document[1000000];
	//char* end = rapidxml::print(document, *doc, 0);

	std::ofstream file((std::ostringstream() << ("Assets/Scenes/" + savedName)).str() + ".xml", std::ios_base::out);
	file << document;
	file.close();
	doc->clear();

	delete doc;
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
