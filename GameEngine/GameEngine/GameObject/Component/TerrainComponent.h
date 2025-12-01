#ifndef TERRAINCOMPONENT_H
#define TERRAINCOMPONENT_H

#include "Component.h"

#include "../../Renderer/GraphicsObjects/GOLit.h"

#include <functional>

class Terrain;
class AxisAlignedBoundingBox;
class LineSegment3D;
class Model;

class TerrainComponent : public Component
{
public:

	TerrainComponent();

	TerrainComponent(const std::string& name, const std::string& heightMapPath, const std::vector<GOLit::Material>& heightMaterials, const std::string& blendMap, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int UVTiling, bool loadAsync);

	~TerrainComponent();

	glm::vec3 GetTerrainPoint(const glm::vec3& position) const;

	const std::vector<std::vector<AxisAlignedBoundingBox*>>& GetCellArray() const;

	void ToggleCells();

	bool ColliderVisible() const;

	void ToggleColliderVisibility();

	glm::vec3 GetLineIntersection(const LineSegment3D& line);

	bool Loaded() const;

	const Model* GetModel() const;

	glm::vec3 GetPosition() const;

	glm::mat4 GetRotation() const;

	glm::mat4 GetTransform() const;

private:

	TerrainComponent(const TerrainComponent&) = delete;

	TerrainComponent& operator=(const TerrainComponent&) = delete;

	TerrainComponent(TerrainComponent&&) = delete;

	TerrainComponent& operator=(TerrainComponent&&) = delete;

	void Update() override;

	void Serialize() override;

	void Deserialize() override;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	Terrain* terrain;
};


#endif //TERRAINCOMPONENT_H