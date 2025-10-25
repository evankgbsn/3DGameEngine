#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <list>

class Model;
class Vertex;

class ModelManager
{
public:

	static void Initialize();

	static void Terminate();

	static Model* const LoadModel(const std::string& name, const std::string& path, bool async);

	static Model* const LoadModel(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, bool async);

	static void UnloadModel(const std::string& name);

	static Model* const GetModel(const std::string& name);

	static bool ModelLoaded(const std::string& name);

	static Model* const LoadModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int UVTiling, bool async);

	static void Update();

	static void RegisterCallbackForModelLoaded(const std::string& modelName, const std::string& callbackName, std::function<void(Model* const)>* callback);

	static void DergisterCallbackForModelLoaded(const std::string& modelName, const std::string& callbackName);

private:

	ModelManager();

	~ModelManager();

	ModelManager(const ModelManager&) = delete;

	ModelManager& operator=(const ModelManager&) = delete;

	ModelManager(ModelManager&&) = delete;

	ModelManager& operator=(ModelManager&&) = delete;

	Model* const InternalLoadModelFromPath(const std::string& name, const std::string& path);

	Model* const InternalLoadModelFromData(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

	Model* const InternalLoadModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int UVTiling);

	void LoadDefaultModels();

	void ProcessLoadedModels();

	static ModelManager* instance;

	std::mutex modelsMutex;

	std::unordered_map<std::string, Model*> models;

	std::mutex loadThreadsMutex;

	std::list<std::thread*> loadThreads;

	std::mutex modelLoadCallbacksMutex;

	std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(Model* const)>*>> modelLoadCallbacks;

	std::mutex internalModelsLoadedMutex;

	std::list<std::string> internalModelsLoaded;

};


#endif // MODELMANAGER_H