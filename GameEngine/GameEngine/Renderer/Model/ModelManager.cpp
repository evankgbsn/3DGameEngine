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

void ModelManager::Update()
{
	if (instance == nullptr)
	{
		return;
	}

	instance->ProcessLoadedModels();
}

void ModelManager::RegisterCallbackForModelLoaded(const std::string& modelName, const std::string& callbackName, std::function<void(Model* const)>* callback)
{
	std::lock_guard<std::mutex> guard(instance->modelLoadCallbacksMutex);

	if (instance == nullptr)
	{
		return;
	}

	const auto& registeredCallback = instance->modelLoadCallbacks[modelName].find(callbackName);
	if (registeredCallback == instance->modelLoadCallbacks[modelName].end())
	{
		instance->modelLoadCallbacks[modelName].insert(std::make_pair(callbackName, callback));
	}
	else
	{
		Logger::Log("Callback with name " + callbackName + " has already been registered for model " + modelName, Logger::Category::Warning);
	}
}

void ModelManager::DergisterCallbackForModelLoaded(const std::string& modelName, const std::string& callbackName)
{
	std::lock_guard<std::mutex> guard(instance->modelLoadCallbacksMutex);

	if (instance->modelLoadCallbacks.find(modelName) ==  instance->modelLoadCallbacks.end())
	{
		return;
	}

	const auto& registeredCallback = instance->modelLoadCallbacks[modelName].find(callbackName);
	if (registeredCallback != instance->modelLoadCallbacks[modelName].end())
	{
		instance->modelLoadCallbacks[modelName].erase(registeredCallback);
	}
	else
	{
		Logger::Log("Callback with name " + callbackName + " has not been registered for model " + modelName, Logger::Category::Warning);
	}
}

Model* const ModelManager::LoadModel(const std::string& name, const std::string& path, bool async)
{
	if (instance == nullptr)
	{
		return nullptr;
	}

	if (async)
	{
		std::lock_guard<std::mutex> guard(instance->loadThreadsMutex);

		std::thread* loadThread = new std::thread(&ModelManager::InternalLoadModelFromPath, instance, name, path);

		instance->loadThreads.push_back(loadThread);

		return nullptr;
	}
	else
	{
		return instance->InternalLoadModelFromPath(name, path);
	}
	

}

Model* const ModelManager::LoadModel(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, bool async)
{
	if (instance == nullptr)
	{
		return nullptr;
	}

	if (async)
	{
		std::lock_guard<std::mutex> guard(instance->loadThreadsMutex);

		std::thread* loadThread = new std::thread(&ModelManager::InternalLoadModelFromData, instance, name, vertices, indices);

		instance->loadThreads.push_back(loadThread);

		return nullptr;
	}
	else
	{
		return instance->InternalLoadModelFromData(name, vertices, indices);
	}
}

void ModelManager::UnloadModel(const std::string& name)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::UnloadModel() before ModelManager::Initialize()."));
		return;
	}

	std::lock_guard<std::mutex> guard(instance->modelsMutex);

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

	std::lock_guard<std::mutex> guard(instance->modelsMutex);

	return instance->models.find(name) != instance->models.end();
}

Model* const ModelManager::LoadModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int UVTiling, bool async)
{
	if (instance == nullptr)
	{
		return nullptr;
	}

	if (async)
	{
		std::lock_guard<std::mutex> guard(instance->loadThreadsMutex);

		std::thread* loadThread = new std::thread(&ModelManager::InternalLoadModelTerrain, instance, name, heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset, UVTiling);

		instance->loadThreads.push_back(loadThread);

		return nullptr;
	}
	else
	{
		return instance->InternalLoadModelTerrain(name, heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset, UVTiling);
	}
}

Model* const ModelManager::InternalLoadModelFromPath(const std::string& name, const std::string& path)
{
	modelsMutex.lock();
	if (models.find(name) != models.end())
	{
		modelsMutex.unlock();

		return models[name];
	}
	else
	{
		modelsMutex.unlock();
	}

	modelsMutex.lock();
	models[name] = new Model(path, name);
	modelsMutex.unlock();

	internalModelsLoadedMutex.lock();
	internalModelsLoaded.push_back(name);
	internalModelsLoadedMutex.unlock();

	return models[name];
}

Model* const ModelManager::InternalLoadModelFromData(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	modelsMutex.lock();

	if (models.find(name) != models.end())
	{

		modelsMutex.unlock();
		std::string glyphName;

		for (unsigned int i = 0; i < 5; i++)
		{
			glyphName += name[i];
		}


		if (glyphName != "Glyph")
		{
			Logger::Log(std::string("Cannot load model with name ") + name + std::string(". This name is already being used."), Logger::Category::Warning);
		}

		return models[name];
	}
	else
	{
		modelsMutex.unlock();
	}

	Model* newModel = new Model(vertices, indices, name);

	modelsMutex.lock();
	models[name] = newModel;
	modelsMutex.unlock();

	internalModelsLoadedMutex.lock();
	internalModelsLoaded.push_back(name);
	internalModelsLoadedMutex.unlock();

	return newModel;
}

Model* const ModelManager::InternalLoadModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int UVTiling)
{
	Model* newTerrainModel = nullptr;

	modelsMutex.lock();

	if (models.find(name) != models.end())
	{
		Logger::Log("The name " + name + " is already being used for a model. ModelManager::CreateModelTerrain()", Logger::Category::Warning);

		modelsMutex.unlock();
		return models[name];
	}
	else
	{
		modelsMutex.unlock();
		newTerrainModel = new Model(heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset, UVTiling, name);

		modelsMutex.lock();
		instance->models.insert(std::pair<std::string, Model*>(name, newTerrainModel));
		modelsMutex.unlock();

		internalModelsLoadedMutex.lock();
		internalModelsLoaded.push_back(name);
		internalModelsLoadedMutex.unlock();

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

	std::lock_guard<std::mutex> guard(instance->modelsMutex);

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
	for (const auto& modelCallbacks : modelLoadCallbacks)
	{
		Model* const model = GetModel(modelCallbacks.first);
		if (model != nullptr)
		{
			for (const auto& callback : modelCallbacks.second)
			{
				if (callback.second != nullptr)
				{
					delete callback.second;
				}
			}
		}
	}

	modelLoadCallbacks.clear();

	for (auto& thread : loadThreads)
	{
		if (thread->joinable())
		{
			thread->join();
			delete thread;
		}
	}

	for (auto& model : models)
	{
		delete model.second;
	}

	instance = nullptr;
}

void ModelManager::LoadDefaultModels()
{
	instance->modelsMutex.lock();

	models.insert(std::make_pair(std::string("Rectangle"), new Model()));
	models["Rectangle"]->CreateVertexArrayBuffer();


	// Default rectangle.
	std::vector<Vertex> rectangleVertices = {
		Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f))
	};

	std::vector<unsigned int> indices = { 0,1,2,2,3,0 };

	Model* rectangleWithDepth = new Model(rectangleVertices, indices, "RectangleWithDepth");
	rectangleWithDepth->CreateVertexArrayBuffer();
	models.insert(std::make_pair(std::string("RectangleWithDepth"), rectangleWithDepth));

	// Default triangle
	std::vector<Vertex> triangleVertices = {
		Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f))
	};

	std::vector<unsigned int> triangleIndices = { 0,2,1 };
	Model* triangle = new Model(triangleVertices, triangleIndices, "Triangle");
	models.insert(std::make_pair(std::string("Triangle"), triangle));

	std::vector<Vertex> nullVertices;
	std::vector<unsigned int> nullIndices;

	Model* null = new Model(nullVertices, nullIndices, "Null");

	models.insert(std::make_pair(std::string("Null"), null));

	instance->modelsMutex.unlock();

	LoadModel("Sphere", "Assets/Model/Sphere.gltf", false);
}

void ModelManager::ProcessLoadedModels()
{
	std::lock_guard<std::mutex> guard(internalModelsLoadedMutex);
	std::lock_guard<std::mutex> guard2(modelLoadCallbacksMutex);

	for (const std::string& name : internalModelsLoaded)
	{
		Model* const loadedModel = GetModel(name);

		loadedModel->CreateVertexArrayBuffer();

		if (modelLoadCallbacks.find(name) != modelLoadCallbacks.end())
		{
			const std::unordered_map<std::string, std::function<void(Model* const)>*>& callbacks = modelLoadCallbacks[name];

			for (const auto& callback : callbacks)
			{
				(*callback.second)(loadedModel);
			}
			
			modelLoadCallbacks.erase(modelLoadCallbacks.find(name));
		}

	}

	internalModelsLoaded.clear();
}
