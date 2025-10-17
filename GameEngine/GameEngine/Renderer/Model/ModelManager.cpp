#include "ModelManager.h"

#include "../../Utils/Logger.h"
#include "Model.h"

#include <stdexcept>
#include <sstream>

ModelManager* ModelManager::instance = nullptr;

void ModelManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new ModelManager();
		instance->LoadDefaultModels();
	}
}

void ModelManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

Model* const ModelManager::LoadModel(const std::string& name, const std::string& path)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::LoadModel() before ModelManager::Initialize()."));
		return nullptr;
	}

	if (instance->models.find(name) != instance->models.end())
	{
		Logger::Log(std::string("Cannot load model with name ") + name + std::string(". This name is already being used."), Logger::Category::Warning);
		return instance->models[name];
	}

	return instance->models[name] = new Model(path, name);
}

Model* const ModelManager::LoadModel(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::LoadModel() before ModelManager::Initialize()."));
		return nullptr;
	}

	if (instance->models.find(name) != instance->models.end())
	{
		std::string glyphName;

		for (unsigned int i = 0; i < 5; i++)
		{
			glyphName += name[i];
		}


		if (glyphName != "Glyph")
		{
			Logger::Log(std::string("Cannot load model with name ") + name + std::string(". This name is already being used."), Logger::Category::Warning);
		}
		return instance->models[name];
	}

	Model* newModel = new Model(vertices, indices, name);
	instance->models[name] = newModel;

	return newModel;
}

void ModelManager::UnloadModel(const std::string& name)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::UnloadModel() before ModelManager::Initialize()."));
		return;
	}

	if (instance->models.find(name) == instance->models.end())
	{
		Logger::Log(std::string("Cannot unload model with name ") + name + std::string(". a model with this name has not been loaded."), Logger::Category::Warning);
		return;
	}

	delete instance->models[name];
	instance->models.erase(instance->models.find(name));
}

bool ModelManager::ModelLoaded(const std::string& name)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::ModelLoaded() before ModelManager::Initialize()."));
		return false;
	}

	return instance->models.find(name) != instance->models.end();
}

Model* const ModelManager::CreateModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset)
{
	Model* newTerrainModel = nullptr;

	if (instance == nullptr)
	{
		Logger::Log("Calling ModelManager::CreateModelTerrain() beofre ModelManager:Initialize()", Logger::Category::Error);
		return newTerrainModel;
	}

	if (instance->models.find(name) != instance->models.end())
	{
		Logger::Log("The name " + name + " is already being used for a model. ModelManager::CreateModelTerrain()", Logger::Category::Warning);
		return instance->models[name];
	}
	else
	{
		newTerrainModel = new Model(heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset, name);
		instance->models.insert(std::pair<std::string, Model*>(name, newTerrainModel));
		return newTerrainModel;
	}
}

Model* const ModelManager::GetModel(const std::string& modelName)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::GetModel() before ModelManager::Initialize()."));
		return nullptr;
	}

	if (instance->models.find(modelName) == instance->models.end())
	{
		Logger::Log(std::string("Could not find model ") + modelName, Logger::Category::Error);
		return nullptr;
	}

	return instance->models[modelName];
}

ModelManager::ModelManager() :
	models(std::unordered_map<std::string, Model*>())
{
	
}

ModelManager::~ModelManager()
{
	for (auto& model : models)
	{
		delete model.second;
	}

	instance = nullptr;
}

void ModelManager::LoadDefaultModels()
{
	models.insert(std::make_pair(std::string("Rectangle"), new Model()));


	// Default rectangle.
	std::vector<Vertex> rectangleVertices = {
		Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f))
	};

	std::vector<unsigned int> indices = { 0,1,2,2,3,0 };

	models.insert(std::make_pair(std::string("RectangleWithDepth"), new Model(rectangleVertices, indices, "RectangleWithDepth")));

	// Default triangle
	std::vector<Vertex> triangleVertices = {
		Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f))
	};

	std::vector<unsigned int> triangleIndices = { 0,2,1 };

	models.insert(std::make_pair(std::string("Triangle"), new Model(triangleVertices, triangleIndices, "Triangle")));

	std::vector<Vertex> nullVertices;
	std::vector<unsigned int> nullIndices;

	models.insert(std::make_pair(std::string("Null"), new Model(nullVertices, nullIndices, "Null")));

	LoadModel("Sphere", "Assets/Model/Sphere.gltf");
}
