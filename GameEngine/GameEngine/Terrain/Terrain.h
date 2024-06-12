#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <list>

class GOTerrain;

class Terrain
{

public:

	struct Cell
	{
		unsigned int x, y;
	};

public:

	Terrain(const std::string& name, const std::string& heightMapPath, const std::string& diffTextureName, const std::string& specTextureName, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset);

	~Terrain();

	void ToggleCells();
	
	void HighlightCell(Cell cell);
	
	Cell TestPoint(glm::vec3 point);
	
	float GetCellHeight(Cell cell);
	
	glm::vec3 GetTerrainPoint(glm::vec3 point);
	
	glm::vec3 GetTerrainNormal(glm::vec3 point);
	
	void VisualizeAllCells();
	
	const std::vector<std::vector<class AxisAlignedBoundingBoxWithVisualization*>>& GetCellArray() const;

private:

	Terrain() = delete;

	Terrain(const Terrain&) = delete;

	Terrain& operator=(const Terrain&) = delete;

	Terrain(Terrain&&) = delete;

	Terrain& operator=(Terrain&&) = delete;

public:

private:

	GOTerrain* terrainGraphics;

	class Model* terrainModel;

	std::vector<std::vector<class AxisAlignedBoundingBoxWithVisualization*>> aabbs;

	std::vector<std::vector<glm::vec3>> tileNormals;

	std::string name;

	unsigned int tileX;

	unsigned int tileY;

	float terrainWidth;

	float terrainHeight;

	float tileWidth;

	float tileHeight;

	float maxHeight;

	float yOffset;

	bool isEnabled;

	std::list<Cell> highlightedCells;

};

#endif // TERRAIN_H