#include "TerrainComponent.h"

#include "../../Terrain/Terrain.h"

TerrainComponent::TerrainComponent(const std::string& name, const std::string& heightMapPath, const std::vector<GOLit::Material>& heightMaterials, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset)
{
	RegisterComponentClassType<TerrainComponent>(this);
	terrain = new Terrain(name, heightMapPath, heightMaterials, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset);
}

TerrainComponent::~TerrainComponent()
{
	delete terrain;
}

glm::vec3 TerrainComponent::GetTerrainPoint(const glm::vec3& position) const
{
	return terrain->GetTerrainPoint(position);
}

const std::vector<std::vector<AxisAlignedBoundingBoxWithVisualization*>>& TerrainComponent::GetCellArray() const
{
	return terrain->GetCellArray();
}

void TerrainComponent::ToggleCells()
{
	terrain->ToggleCells();
}

void TerrainComponent::Update()
{
}

const std::vector<char> TerrainComponent::Serialize() const
{
	return std::vector<char>();
}

void TerrainComponent::Deserialize(const std::vector<char>& data)
{
}
