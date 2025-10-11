#ifndef TERRAIN_H
#define TERRAIN_H

#include "../Renderer/GraphicsObjects/GOLit.h"
#include "../Math/Shapes/Ray.h"

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

	Terrain(const std::string& name, const std::string& heightMapPath, const std::vector<GOLit::Material>& heightMaterials, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset);

	~Terrain();

	bool IsEnabled() const;

	bool IsVisible(const Cell& cell) const;

	void ToggleCells();

	void ToggleCell(const Cell& cell);
	
	void HighlightCell(const Cell& cell, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});
	
	Cell TestPoint(const glm::vec3& point) const;
	
	float GetCellHeight(const Cell& cell);
	
	glm::vec3 GetTerrainPoint(const glm::vec3& point);
	
	glm::vec3 GetTerrainNormal(const glm::vec3& point);

	void UpdateHeightByTerrainPoint(const glm::vec3& point, float heightChange);
	
	void VisualizeAllCells();
	
	const std::vector<std::vector<class AxisAlignedBoundingBox*>>& GetCellArray() const;

	void UpdateTerrainCells();

	const std::string& GetHeightMapPath() const;

	const std::string& GetName() const;

	float GetWidth() const;

	float GetHeight() const;

	unsigned int GetTileX() const;

	unsigned int GetTileY() const;

	float GetMaxHeight() const;

	float GetYOffset() const;

	const std::vector<GOLit::Material>& GetMaterials() const;

	GOTerrain* GetGraphics() const;

	glm::vec3 RayIntersect(const Ray& ray) const;

private:

	Terrain() = delete;

	Terrain(const Terrain&) = delete;

	Terrain& operator=(const Terrain&) = delete;

	Terrain(Terrain&&) = delete;

	Terrain& operator=(Terrain&&) = delete;

	void CreateAABBs();

public:

private:

	GOTerrain* terrainGraphics;

	class Model* terrainModel;

	std::vector<std::vector<class AxisAlignedBoundingBox*>> aabbs;

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

	std::string heightMapPath;

};

#endif // TERRAIN_H