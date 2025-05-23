#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <string>
#include <unordered_map>

class Model;
class Vertex;

class ModelManager
{
public:

	static void Initialize();

	static void Terminate();

	static Model* const LoadModel(const std::string& name, const std::string& path);

	static Model* const LoadModel(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

	static void UnloadModel(const std::string& name);

	static Model* const GetModel(const std::string& name);

	static bool ModelLoaded(const std::string& name);

	static Model* const CreateModelTerrain(const std::string& name, const std::string& heightMapPath, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset);

private:

	ModelManager();

	~ModelManager();

	ModelManager(const ModelManager&) = delete;

	ModelManager& operator=(const ModelManager&) = delete;

	ModelManager(ModelManager&&) = delete;

	ModelManager& operator=(ModelManager&&) = delete;

	void LoadDefaultModels();

	static ModelManager* instance;

	std::unordered_map<std::string, Model*> models;

};


#endif // MODELMANAGER_H