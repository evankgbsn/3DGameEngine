#include "Scene.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/Component/Component.h"
#include "../Editor/Editor.h"

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
	initialized(false),
	loaded(false)
{
}

Scene::~Scene()
{
	for (const auto& obj : createdObjects)
	{
		delete obj.second;
	}
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

		object->SetName(name);

		object->owningScene = this;

		if (loaded)
		{
			object->Load();
		}

		if (initialized)
		{
			object->Initialize();
		}

		if (started)
		{
			object->Start();
		}
	}
}

void Scene::DeregisterGameObject(const std::string& name)
{
	if (objects.find(name) != objects.end())
	{
		const auto& object = objects.find(name);

		if (started)
		{
			object->second->End();
		}

		if (initialized)
		{
			object->second->Terminate();
		}

		if (loaded)
		{
			object->second->Unload();
		}

		object->second->owningScene = nullptr;
		objects.erase(object);
	}
}

void Scene::Initialize()
{
	for (auto& gameObject : objects)
	{
		if (gameObject.second != nullptr)
		{
			gameObject.second->Initialize();
		}
	}

	initialized = true;
}

void Scene::Terminate()
{
	for (auto& gameObject : objects)
	{
		if (gameObject.second != nullptr)
		{
			gameObject.second->Terminate();
		}
	}

	initialized = false;
}

void Scene::Load()
{
	LoadObjects();

	loaded = true;
}

void Scene::Unload()
{
	UnloadObjects();

	loaded = false;
}

void Scene::Deserialize(const std::string& path)
{
	if (Started())
	{
		End();
	}

	if (Initialized())
	{
		Terminate();
	}

	Initialize();

	// Open and parse the xml document.
	rapidxml::file<>* xmlFile = new rapidxml::file<>(path.c_str());
	rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
	doc->parse<0>(xmlFile->data());
	rapidxml::xml_node<>* n = doc->first_node();


	std::string type = n->first_attribute("type")->value();
	std::string name = n->first_attribute("name")->value();

	// Get all the objects.
	n = n->first_node();
	while (n != nullptr)
	{
		std::string name = n->first_attribute("name")->value();
		std::string type = n->first_attribute("type")->value();

		// Check if the object is already in the scene.
		if (objects.find(name) == objects.end())
		{
			// Create the new object and add it to the objects.
			std::function<void(GameObject**)> constructor = GameObject::GetConstructor(type);

			GameObject* newObj;
			constructor(&newObj);

			createdObjects[name] = newObj;

			RegisterGameObject(newObj, name);

			// Update all its components.
			rapidxml::xml_node<>* componentNode = n->first_node();
			while (componentNode != nullptr)
			{
				std::string componentName = componentNode->first_attribute("name")->value();
				std::string componenType = componentNode->first_attribute("type")->value();

				if (newObj->components.find(componentName) == newObj->components.end())
				{
					// Create the new component and add it to the components.
					std::function<void(Component**)> componentConstructor = Component::GetConstructor(componenType);
					
					Component* newComponent;
					componentConstructor(&newComponent);

					newObj->AddComponent(newComponent, componentName);

					// Update all its valiables
					rapidxml::xml_node<>* variableNode = componentNode->first_node();
					while (variableNode != nullptr)
					{
						if (!strcmp(variableNode->first_attribute("type")->value(), "vec3"))
						{
							rapidxml::xml_node<>* vec3 = variableNode->first_node();
							glm::vec3 data;
							data.x = std::stof(vec3->first_attribute("X")->value());
							vec3 = vec3->next_sibling();
							data.y = std::stof(vec3->first_attribute("Y")->value());
							vec3 = vec3->next_sibling();
							data.z = std::stof(vec3->first_attribute("Z")->value());

							newComponent->savedVec3s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "int"))
						{
							int data;
							data = std::stoi(variableNode->first_node("int")->first_attribute("val")->value());
							newComponent->savedInts[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "float"))
						{
							float data;
							data = std::stof(variableNode->first_node("float")->first_attribute("val")->value());
							newComponent->savedFloats[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec4"))
						{
							rapidxml::xml_node<>* vec4 = variableNode->first_node();
							glm::vec4 data;
							data.x = std::stof(vec4->first_attribute("X")->value());
							vec4 = vec4->next_sibling();
							data.y = std::stof(vec4->first_attribute("Y")->value());
							vec4 = vec4->next_sibling();
							data.z = std::stof(vec4->first_attribute("Z")->value());
							vec4 = vec4->next_sibling();
							data.w = std::stof(vec4->first_attribute("W")->value());

							newComponent->savedVec4s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "string"))
						{
							std::string data;
							data = variableNode->first_node("string")->first_attribute("val")->value();
							newComponent->savedStrings[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "bool"))
						{
							bool data;
							data = std::stoi(variableNode->first_node("bool")->first_attribute("val")->value());
							newComponent->savedBools[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec2"))
						{
							rapidxml::xml_node<>* vec2 = variableNode->first_node();
							glm::vec2 data;
							data.x = std::stof(vec2->first_attribute("X")->value());
							vec2 = vec2->next_sibling();
							data.y = std::stof(vec2->first_attribute("Y")->value());

							newComponent->savedVec2s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "mat4"))
						{
							rapidxml::xml_node<>* mat4 = variableNode->first_node();
							glm::mat4 data;
							data[0][0] = std::stof(mat4->first_attribute("XX")->value());
							mat4 = mat4->next_sibling();
							data[0][1] = std::stof(mat4->first_attribute("XY")->value());
							mat4 = mat4->next_sibling();
							data[0][2] = std::stof(mat4->first_attribute("XZ")->value());
							mat4 = mat4->next_sibling();
							data[0][3] = std::stof(mat4->first_attribute("XW")->value());
							mat4 = mat4->next_sibling();

							data[1][0] = std::stof(mat4->first_attribute("YX")->value());
							mat4 = mat4->next_sibling();
							data[1][1] = std::stof(mat4->first_attribute("YY")->value());
							mat4 = mat4->next_sibling();
							data[1][2] = std::stof(mat4->first_attribute("YZ")->value());
							mat4 = mat4->next_sibling();
							data[1][3] = std::stof(mat4->first_attribute("YW")->value());
							mat4 = mat4->next_sibling();

							data[2][0] = std::stof(mat4->first_attribute("ZX")->value());
							mat4 = mat4->next_sibling();
							data[2][1] = std::stof(mat4->first_attribute("ZY")->value());
							mat4 = mat4->next_sibling();
							data[2][2] = std::stof(mat4->first_attribute("ZZ")->value());
							mat4 = mat4->next_sibling();
							data[2][3] = std::stof(mat4->first_attribute("ZW")->value());
							mat4 = mat4->next_sibling();

							data[3][0] = std::stof(mat4->first_attribute("WX")->value());
							mat4 = mat4->next_sibling();
							data[3][1] = std::stof(mat4->first_attribute("WY")->value());
							mat4 = mat4->next_sibling();
							data[3][2] = std::stof(mat4->first_attribute("WZ")->value());
							mat4 = mat4->next_sibling();
							data[3][3] = std::stof(mat4->first_attribute("WW")->value());

							newComponent->savedMat4s[variableNode->first_attribute("name")->value()] = data;
						}

						variableNode = variableNode->next_sibling("Variable");
					}

					//newComponent->Deserialize();
				}
				else
				{
					Component* newComponent = newObj->components.find(componentName)->second;

					// Update all its valiables
					rapidxml::xml_node<>* variableNode = componentNode->first_node();
					while (variableNode != nullptr)
					{
						if (!strcmp(variableNode->first_attribute("type")->value(), "vec3"))
						{
							rapidxml::xml_node<>* vec3 = variableNode->first_node();
							glm::vec3 data;
							data.x = std::stof(vec3->first_attribute("X")->value());
							vec3 = vec3->next_sibling();
							data.y = std::stof(vec3->first_attribute("Y")->value());
							vec3 = vec3->next_sibling();
							data.z = std::stof(vec3->first_attribute("Z")->value());

							newComponent->savedVec3s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "int"))
						{
							int data;
							data = std::stoi(variableNode->first_node("int")->first_attribute("val")->value());
							newComponent->savedInts[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "float"))
						{
							float data;
							data = std::stof(variableNode->first_node("float")->first_attribute("val")->value());
							newComponent->savedFloats[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec4"))
						{
							rapidxml::xml_node<>* vec4 = variableNode->first_node();
							glm::vec4 data;
							data.x = std::stof(vec4->first_attribute("X")->value());
							vec4 = vec4->next_sibling();
							data.y = std::stof(vec4->first_attribute("Y")->value());
							vec4 = vec4->next_sibling();
							data.z = std::stof(vec4->first_attribute("Z")->value());
							vec4 = vec4->next_sibling();
							data.w = std::stof(vec4->first_attribute("W")->value());

							newComponent->savedVec4s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "string"))
						{
							std::string data;
							data = variableNode->first_node("string")->first_attribute("val")->value();
							newComponent->savedStrings[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "bool"))
						{
							bool data;
							data = std::stoi(variableNode->first_node("bool")->first_attribute("val")->value());
							newComponent->savedBools[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec2"))
						{
							rapidxml::xml_node<>* vec2 = variableNode->first_node();
							glm::vec2 data;
							data.x = std::stof(vec2->first_attribute("X")->value());
							vec2 = vec2->next_sibling();
							data.y = std::stof(vec2->first_attribute("Y")->value());

							newComponent->savedVec2s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "mat4"))
						{
							rapidxml::xml_node<>* mat4 = variableNode->first_node();
							glm::mat4 data;
							data[0][0] = std::stof(mat4->first_attribute("XX")->value());
							mat4 = mat4->next_sibling();
							data[0][1] = std::stof(mat4->first_attribute("XY")->value());
							mat4 = mat4->next_sibling();
							data[0][2] = std::stof(mat4->first_attribute("XZ")->value());
							mat4 = mat4->next_sibling();
							data[0][3] = std::stof(mat4->first_attribute("XW")->value());
							mat4 = mat4->next_sibling();

							data[1][0] = std::stof(mat4->first_attribute("YX")->value());
							mat4 = mat4->next_sibling();
							data[1][1] = std::stof(mat4->first_attribute("YY")->value());
							mat4 = mat4->next_sibling();
							data[1][2] = std::stof(mat4->first_attribute("YZ")->value());
							mat4 = mat4->next_sibling();
							data[1][3] = std::stof(mat4->first_attribute("YW")->value());
							mat4 = mat4->next_sibling();

							data[2][0] = std::stof(mat4->first_attribute("ZX")->value());
							mat4 = mat4->next_sibling();
							data[2][1] = std::stof(mat4->first_attribute("ZY")->value());
							mat4 = mat4->next_sibling();
							data[2][2] = std::stof(mat4->first_attribute("ZZ")->value());
							mat4 = mat4->next_sibling();
							data[2][3] = std::stof(mat4->first_attribute("ZW")->value());
							mat4 = mat4->next_sibling();

							data[3][0] = std::stof(mat4->first_attribute("WX")->value());
							mat4 = mat4->next_sibling();
							data[3][1] = std::stof(mat4->first_attribute("WY")->value());
							mat4 = mat4->next_sibling();
							data[3][2] = std::stof(mat4->first_attribute("WZ")->value());
							mat4 = mat4->next_sibling();
							data[3][3] = std::stof(mat4->first_attribute("WW")->value());

							newComponent->savedMat4s[variableNode->first_attribute("name")->value()] = data;
						}

						variableNode = variableNode->next_sibling("Variable");
					}

					//newComponent->Deserialize();
				}

				componentNode = componentNode->next_sibling("Component");
			}

			newObj->Deserialize();
		}
		else
		{
			GameObject* go = objects.find(name)->second;

			GameObject* newObj = go;

			newObj->Terminate();
			newObj->Initialize();

			// Update all its components.
			rapidxml::xml_node<>* componentNode = n->first_node();
			while (componentNode != nullptr)
			{
				std::string componentName = componentNode->first_attribute("name")->value();
				std::string componenType = componentNode->first_attribute("type")->value();

				if (newObj->components.find(componentName) == newObj->components.end())
				{
					// Create the new component and add it to the components.
					std::function<void(Component**)> componentConstructor = Component::GetConstructor(name);

					Component* newComponent;
					componentConstructor(&newComponent);

					newObj->AddComponent(newComponent, componentName);

					// Update all its valiables
					rapidxml::xml_node<>* variableNode = componentNode->first_node();
					while (variableNode != nullptr)
					{
						if (!strcmp(variableNode->first_attribute("type")->value(), "vec3"))
						{
							rapidxml::xml_node<>* vec3 = variableNode->first_node();
							glm::vec3 data;
							data.x = std::stof(vec3->first_attribute("X")->value());
							vec3 = vec3->next_sibling();
							data.y = std::stof(vec3->first_attribute("Y")->value());
							vec3 = vec3->next_sibling();
							data.z = std::stof(vec3->first_attribute("Z")->value());

							newComponent->savedVec3s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "int"))
						{
							int data;
							data = std::stoi(variableNode->first_node("int")->first_attribute("val")->value());
							newComponent->savedInts[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "float"))
						{
							float data;
							data = std::stof(variableNode->first_node("float")->first_attribute("val")->value());
							newComponent->savedFloats[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec4"))
						{
							rapidxml::xml_node<>* vec4 = variableNode->first_node();
							glm::vec4 data;
							data.x = std::stof(vec4->first_attribute("X")->value());
							vec4 = vec4->next_sibling();
							data.y = std::stof(vec4->first_attribute("Y")->value());
							vec4 = vec4->next_sibling();
							data.z = std::stof(vec4->first_attribute("Z")->value());
							vec4 = vec4->next_sibling();
							data.w = std::stof(vec4->first_attribute("W")->value());

							newComponent->savedVec4s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "string"))
						{
							std::string data;
							data = variableNode->first_node("string")->first_attribute("val")->value();
							newComponent->savedStrings[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "bool"))
						{
							bool data;
							data = std::stoi(variableNode->first_node("bool")->first_attribute("val")->value());
							newComponent->savedBools[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec2"))
						{
							rapidxml::xml_node<>* vec2 = variableNode->first_node();
							glm::vec2 data;
							data.x = std::stof(vec2->first_attribute("X")->value());
							vec2 = vec2->next_sibling();
							data.y = std::stof(vec2->first_attribute("Y")->value());

							newComponent->savedVec2s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "mat4"))
						{
							rapidxml::xml_node<>* mat4 = variableNode->first_node();
							glm::mat4 data;
							data[0][0] = std::stof(mat4->first_attribute("XX")->value());
							mat4 = mat4->next_sibling();
							data[0][1] = std::stof(mat4->first_attribute("XY")->value());
							mat4 = mat4->next_sibling();
							data[0][2] = std::stof(mat4->first_attribute("XZ")->value());
							mat4 = mat4->next_sibling();
							data[0][3] = std::stof(mat4->first_attribute("XW")->value());
							mat4 = mat4->next_sibling();

							data[1][0] = std::stof(mat4->first_attribute("YX")->value());
							mat4 = mat4->next_sibling();
							data[1][1] = std::stof(mat4->first_attribute("YY")->value());
							mat4 = mat4->next_sibling();
							data[1][2] = std::stof(mat4->first_attribute("YZ")->value());
							mat4 = mat4->next_sibling();
							data[1][3] = std::stof(mat4->first_attribute("YW")->value());
							mat4 = mat4->next_sibling();

							data[2][0] = std::stof(mat4->first_attribute("ZX")->value());
							mat4 = mat4->next_sibling();
							data[2][1] = std::stof(mat4->first_attribute("ZY")->value());
							mat4 = mat4->next_sibling();
							data[2][2] = std::stof(mat4->first_attribute("ZZ")->value());
							mat4 = mat4->next_sibling();
							data[2][3] = std::stof(mat4->first_attribute("ZW")->value());
							mat4 = mat4->next_sibling();

							data[3][0] = std::stof(mat4->first_attribute("WX")->value());
							mat4 = mat4->next_sibling();
							data[3][1] = std::stof(mat4->first_attribute("WY")->value());
							mat4 = mat4->next_sibling();
							data[3][2] = std::stof(mat4->first_attribute("WZ")->value());
							mat4 = mat4->next_sibling();
							data[3][3] = std::stof(mat4->first_attribute("WW")->value());

							newComponent->savedMat4s[variableNode->first_attribute("name")->value()] = data;
						}

						variableNode = variableNode->next_sibling("Variable");
					}

					//newComponent->Deserialize();
				}
				else
				{
					Component* newComponent = newObj->components.find(componentName)->second;

					// Update all its valiables
					rapidxml::xml_node<>* variableNode = componentNode->first_node();
					while (variableNode != nullptr)
					{
						if (!strcmp(variableNode->first_attribute("type")->value(), "vec3"))
						{
							rapidxml::xml_node<>* vec3 = variableNode->first_node();
							glm::vec3 data;
							data.x = std::stof(vec3->first_attribute("X")->value());
							vec3 = vec3->next_sibling();
							data.y = std::stof(vec3->first_attribute("Y")->value());
							vec3 = vec3->next_sibling();
							data.z = std::stof(vec3->first_attribute("Z")->value());

							newComponent->savedVec3s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "int"))
						{
							int data;
							data = std::stoi(variableNode->first_node("int")->first_attribute("val")->value());
							newComponent->savedInts[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "float"))
						{
							float data;
							data = std::stof(variableNode->first_node("float")->first_attribute("val")->value());
							newComponent->savedFloats[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec4"))
						{
							rapidxml::xml_node<>* vec4 = variableNode->first_node();
							glm::vec4 data;
							data.x = std::stof(vec4->first_attribute("X")->value());
							vec4 = vec4->next_sibling();
							data.y = std::stof(vec4->first_attribute("Y")->value());
							vec4 = vec4->next_sibling();
							data.z = std::stof(vec4->first_attribute("Z")->value());
							vec4 = vec4->next_sibling();
							data.w = std::stof(vec4->first_attribute("W")->value());

							newComponent->savedVec4s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "string"))
						{
							std::string data;
							data = variableNode->first_node("string")->first_attribute("val")->value();
							newComponent->savedStrings[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "bool"))
						{
							bool data;
							data = std::stoi(variableNode->first_node("bool")->first_attribute("val")->value());
							newComponent->savedBools[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "vec2"))
						{
							rapidxml::xml_node<>* vec2 = variableNode->first_node();
							glm::vec2 data;
							data.x = std::stof(vec2->first_attribute("X")->value());
							vec2 = vec2->next_sibling();
							data.y = std::stof(vec2->first_attribute("Y")->value());

							newComponent->savedVec2s[variableNode->first_attribute("name")->value()] = data;
						}
						else if (!strcmp(variableNode->first_attribute("type")->value(), "mat4"))
						{
							rapidxml::xml_node<>* mat4 = variableNode->first_node();
							glm::mat4 data;
							data[0][0] = std::stof(mat4->first_attribute("XX")->value());
							mat4 = mat4->next_sibling();
							data[0][1] = std::stof(mat4->first_attribute("XY")->value());
							mat4 = mat4->next_sibling();
							data[0][2] = std::stof(mat4->first_attribute("XZ")->value());
							mat4 = mat4->next_sibling();
							data[0][3] = std::stof(mat4->first_attribute("XW")->value());
							mat4 = mat4->next_sibling();

							data[1][0] = std::stof(mat4->first_attribute("YX")->value());
							mat4 = mat4->next_sibling();
							data[1][1] = std::stof(mat4->first_attribute("YY")->value());
							mat4 = mat4->next_sibling();
							data[1][2] = std::stof(mat4->first_attribute("YZ")->value());
							mat4 = mat4->next_sibling();
							data[1][3] = std::stof(mat4->first_attribute("YW")->value());
							mat4 = mat4->next_sibling();

							data[2][0] = std::stof(mat4->first_attribute("ZX")->value());
							mat4 = mat4->next_sibling();
							data[2][1] = std::stof(mat4->first_attribute("ZY")->value());
							mat4 = mat4->next_sibling();
							data[2][2] = std::stof(mat4->first_attribute("ZZ")->value());
							mat4 = mat4->next_sibling();
							data[2][3] = std::stof(mat4->first_attribute("ZW")->value());
							mat4 = mat4->next_sibling();

							data[3][0] = std::stof(mat4->first_attribute("WX")->value());
							mat4 = mat4->next_sibling();
							data[3][1] = std::stof(mat4->first_attribute("WY")->value());
							mat4 = mat4->next_sibling();
							data[3][2] = std::stof(mat4->first_attribute("WZ")->value());
							mat4 = mat4->next_sibling();
							data[3][3] = std::stof(mat4->first_attribute("WW")->value());

							newComponent->savedMat4s[variableNode->first_attribute("name")->value()] = data;
						}

						variableNode = variableNode->next_sibling("Variable");
					}

					//newComponent->Deserialize();
				}

				componentNode = componentNode->next_sibling("Component");
			}
			

			newObj->Deserialize();
		}

		// Go to the next object.
		n = n->next_sibling("Object");
	}
	// Clean up memory allocated for xml file.
	delete doc;
	delete xmlFile;
	Logger::Log("Deserialized Scene: " + name, Logger::Category::Success);
}

const std::string& Scene::GetName() const
{
	return name;
}

bool Scene::Loaded() const
{
	return loaded;
}

bool Scene::Started() const
{
	return started;
}

bool Scene::Initialized() const
{
	return initialized;
}

void Scene::AddToCreatedObjects(const std::string& name, GameObject* obj)
{
	createdObjects[name] = obj;
}

void Scene::RemoveFromCreatedObjects(const std::string& name)
{
	createdObjects.erase(createdObjects.find(name));
}

void Scene::Save(const std::string& saveFileName)
{
	std::vector<std::string> temp;
	temp.reserve(10000);

	// Create a document.
	rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
	rapidxml::xml_node<>* decl = doc->allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc->allocate_attribute("version", "1.0"));
	decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
	doc->append_node(decl);

	// Make the scene root node.
	rapidxml::xml_node<>* scene = doc->allocate_node(rapidxml::node_element, "Scene");
	rapidxml::xml_attribute<>* name = doc->allocate_attribute("type", typeid(*this).name());
	rapidxml::xml_attribute<>* type = doc->allocate_attribute("name", saveFileName.c_str());
	scene->append_attribute(type);
	scene->append_attribute(name);
	doc->append_node(scene);

	for (auto& obj : GetGameObjects())
	{
		obj.second->Serialize();

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

			for (const auto& data : comp.second->savedFloats)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "float");
				var->append_attribute(varType);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second));
				varValueValue = doc->allocate_attribute("val", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				var->append_attribute(varName);
				component->append_node(var);
			}

			for (const auto& data : comp.second->savedInts)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "int");
				var->append_attribute(varType);
				varValue = doc->allocate_node(rapidxml::node_element, "int");
				temp.push_back(std::to_string(data.second));
				varValueValue = doc->allocate_attribute("val", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				var->append_attribute(varName);
				component->append_node(var);
			}

			for (const auto& data : comp.second->savedBools)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "bool");
				var->append_attribute(varType);
				varValue = doc->allocate_node(rapidxml::node_element, "bool");

				temp.push_back(std::to_string(data.second));
				varValueValue = doc->allocate_attribute("val", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				var->append_attribute(varName);
				component->append_node(var);
			}

			for (const auto& data : comp.second->savedVec2s)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "vec2");
				var->append_attribute(varType);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.x));
				varValueValue = doc->allocate_attribute("X", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.y));
				varValueValue = doc->allocate_attribute("Y", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				var->append_attribute(varName);
				component->append_node(var);
			}

			for (const auto& data : comp.second->savedVec3s)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "vec3");
				var->append_attribute(varType);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.x));
				varValueValue = doc->allocate_attribute("X", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.y));
				varValueValue = doc->allocate_attribute("Y", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.z));
				varValueValue = doc->allocate_attribute("Z", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				var->append_attribute(varName);
				component->append_node(var);
			}

			for (const auto& data : comp.second->savedVec4s)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "vec4");
				var->append_attribute(varType);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.x));
				varValueValue = doc->allocate_attribute("X", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.y));
				varValueValue = doc->allocate_attribute("Y", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.z));
				varValueValue = doc->allocate_attribute("Z", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second.w));
				varValueValue = doc->allocate_attribute("W", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				var->append_attribute(varName);
				component->append_node(var);
			}

			for (const auto& data : comp.second->savedMat4s)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "mat4");
				var->append_attribute(varType);

				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[0][0]));
				varValueValue = doc->allocate_attribute("XX", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[0][1]));
				varValueValue = doc->allocate_attribute("XY", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[0][2]));
				varValueValue = doc->allocate_attribute("XZ", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[0][3]));
				varValueValue = doc->allocate_attribute("XW", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);

				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[1][0]));
				varValueValue = doc->allocate_attribute("YX", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[1][1]));
				varValueValue = doc->allocate_attribute("YY", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[1][2]));
				varValueValue = doc->allocate_attribute("YZ", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[1][3]));
				varValueValue = doc->allocate_attribute("YW", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);

				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[2][0]));
				varValueValue = doc->allocate_attribute("ZX", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[2][1]));
				varValueValue = doc->allocate_attribute("ZY", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[2][2]));
				varValueValue = doc->allocate_attribute("ZZ", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[2][3]));
				varValueValue = doc->allocate_attribute("ZW", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);

				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[3][0]));
				varValueValue = doc->allocate_attribute("WX", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[3][1]));
				varValueValue = doc->allocate_attribute("WY", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[3][2]));
				varValueValue = doc->allocate_attribute("WZ", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				varValue = doc->allocate_node(rapidxml::node_element, "float");
				temp.push_back(std::to_string(data.second[3][3]));
				varValueValue = doc->allocate_attribute("WW", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);

				var->append_attribute(varName);
				component->append_node(var);
			}

			for (const auto& data : comp.second->savedStrings)
			{
				rapidxml::xml_node<>* var = doc->allocate_node(rapidxml::node_element, "Variable");
				rapidxml::xml_attribute<>* varName = doc->allocate_attribute("name", data.first.c_str());
				rapidxml::xml_attribute<>* varType;

				rapidxml::xml_node<>* varValue;
				rapidxml::xml_attribute<>* varValueValue;

				varType = doc->allocate_attribute("type", "string");
				var->append_attribute(varType);
				varValue = doc->allocate_node(rapidxml::node_element, "string");
				temp.push_back(data.second);
				varValueValue = doc->allocate_attribute("val", temp[temp.size() - 1].c_str());
				varValue->append_attribute(varValueValue);
				var->append_node(varValue);
				var->append_attribute(varName);
				component->append_node(var);
			}

			object->append_node(component);
		}

		scene->append_node(object);
	}

	// Convert the document to a string.
	std::string document;
	rapidxml::print(std::back_inserter(document), *doc);
	//char document[1000000];
	//char* end = rapidxml::print(document, *doc, 0);

	std::string path = "Assets/Scenes/" + saveFileName;

	std::ofstream file((std::ostringstream() << (path)).str(), std::ios_base::out);
	file << document;
	file.close();
	doc->clear();

	delete doc;

	Logger::Log(std::string("Saved Scene: ") + path, Logger::Category::Success);
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
		object.second->Initialize();
	}
}

void Scene::TerminateObjects()
{
	for (auto& object : objects)
	{
		object.second->Terminate();
	}
}

void Scene::LoadObjects()
{
	for (auto& object : objects)
	{
		object.second->Load();
	}
}

void Scene::UnloadObjects()
{
	for (auto& object : objects)
	{
		object.second->Unload();
	}
}
