#ifndef TERRAINCOMPONENT_H
#define TERRAINCOMPONENT_H

#include "Component.h"

#include "../../Renderer/GraphicsObjects/GOLit.h"

class Terrain;
class AxisAlignedBoundingBoxWithVisualization;

class TerrainComponent : public Component
{
public:

	TerrainComponent();

	TerrainComponent(const std::string& name, const std::string& heightMapPath, const std::vector<GOLit::Material>& heightMaterials, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset);

	~TerrainComponent();

	glm::vec3 GetTerrainPoint(const glm::vec3& position) const;

	const std::vector<std::vector<AxisAlignedBoundingBoxWithVisualization*>>& GetCellArray() const;

	void ToggleCells();

private:

	TerrainComponent(const TerrainComponent&) = delete;

	TerrainComponent& operator=(const TerrainComponent&) = delete;

	TerrainComponent(TerrainComponent&&) = delete;

	TerrainComponent& operator=(TerrainComponent&&) = delete;

	void Update() override;

	void Serialize() override;

	void Deserialize() override;

	Terrain* terrain;

};


#endif //TERRAINCOMPONENT_H