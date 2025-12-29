#ifndef MODEL_H
#define MODEL_H

#include "Vertex.h"

#include "../Animation/Clip.h"

#include <string>
#include <vector>
#include <mutex>

class Pose;
class Armature;
class BakedAnimation;

struct cgltf_data;
struct cgltf_attribute;
struct cgltf_skin;
struct cgltf_node;

class Model
{

public:

	Model();

	Model(const std::vector<Vertex>& verices, const std::vector<unsigned int>& indices, const std::string& name);

	Model(const std::string& path, const std::string& name);

	Model(const std::string& heightMapPath, float terrainWidth, float teraainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int UVTiling, const std::string& name);

	~Model();

	Model(const Model&) = delete;

	Model& operator=(const Model&) = delete;

	Model(Model&&) = delete;

	Model& operator=(Model&&) = delete;

	const std::vector<Vertex>& GetVertices() const;

	std::vector<Vertex>& GetVertices();

	const std::vector<unsigned int>& GetIndices() const;

	Armature* const GetArmature() const;

	const std::vector<FastClip>& GetAnimationClips() const;

	FastClip* GetAnimationClip(unsigned int index);

	const BakedAnimation& GetBakedAnimation(unsigned int index) const;

	void CPUSkin(Armature& armature, Pose& pose);

	void SetZforAllVerts(float newZ);

	void FlipTriangleWindingOrder();

	void BindBuffer();

	void UpdateBuffer();

	const std::string& GetName() const;

protected:

private:

	friend class ModelManager;

	void CPUSkinMatrices(Armature& armature, Pose& pose);

	void LoadAnimationClips(cgltf_data* data);

	void BakeAnimations();

	void ModelFromAttribute(cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodes, unsigned int nodeCount);

	void LoadMeshFromGLTF(cgltf_data* data);

	void CreateVertexArrayBuffer();

	void DestroyVertexArrayBuffer();

	void CreateTerrain(const std::string& heightMapPath, float terrainWidth, float teraainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int UVTiling, const std::string& name);

	void CreateTerrainLegacy(const std::string& heightMapPath, float terrainWidth, float teraainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, const std::string& name);

	std::vector<Vertex> vertices;

	std::vector<unsigned int> indices;

	Armature* armature;

	std::vector<FastClip> animationClips;

	std::vector<BakedAnimation> bakedAnimations;

	std::vector<glm::vec3> skinnedPosition;

	std::vector<glm::vec3> skinnedNormal;

	std::vector<glm::mat4> posePalette;

	unsigned int vertexArrayBuffer;

	unsigned int indexArrayBuffer;

	std::string name;

	static std::mutex loadMutex;

};

#endif // MODEL_H