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

	static Model* const LoadModel(const std::string& name, const std::string& path, bool async, std::function<void(Model* const)> callback = [](Model* const) {});

	static Model* const LoadModel(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, bool async, std::function<void(Model* const)> callback = [](Model* const) {});

	static void UnloadModel(const std::string& name);

	static Model* const GetModel(const std::string& name);

	static bool ModelLoaded(const std::string& name);

	static Model* const LoadModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, bool async, std::function<void(Model* const)> callback = [](Model* const) {});

	static void Update();

private:

	ModelManager();

	~ModelManager();

	ModelManager(const ModelManager&) = delete;

	ModelManager& operator=(const ModelManager&) = delete;

	ModelManager(ModelManager&&) = delete;

	ModelManager& operator=(ModelManager&&) = delete;

	Model* const InternalLoadModelFromPath(const std::string& name, const std::string& path, std::function<void(Model* const)> callback = [](Model* const) {});

	Model* const InternalLoadModelFromData(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, std::function<void(Model* const)> callback = [](Model* const) {});

	Model* const InternalLoadModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, std::function<void(Model* const)> callback = [](Model* const) {});

	void LoadDefaultModels();

	static ModelManager* instance;

	std::mutex modelsMutex;

	std::unordered_map<std::string, Model*> models;

	std::mutex loadThreadsMutex;

	std::list<std::thread*> loadThreads;

	std::mutex modelLoadCallbacksMutex;

	std::list<std::function<void()>> modelLoadCallbacks;

};


#endif // MODELMANAGER_H