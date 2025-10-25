#ifndef TERRAIN_H
#define TERRAIN_H

#include "../Renderer/GraphicsObjects/GOLit.h"
#include "../Math/Shapes/Ray.h"

#include <string>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <list>
#include <atomic>
#include <thread>
#include <functional>

class GO3D;
class Model;

class Terrain
{

public:

	struct Cell
	{
		unsigned int x, y;
	};

public:

	Terrain(const std::string& name, const std::string& heightMapPath, const std::vector<GOLit::Material>& heightMaterials, const std::string& blendMap, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset, unsigned int uvTiling, bool isWater = false);

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
	
	void VisualizeAllCells();
	
	const std::vector<std::vector<class AxisAlignedBoundingBox*>>& GetCellArray() const;

	void UpdateTerrainCells();

	const std::string& GetHeightMapPath() const;

	const std::string& GetName() const;

	const std::string& GetBlendMapName() const;

	float GetWidth() const;

	float GetHeight() const;

	unsigned int GetTileX() const;

	unsigned int GetTileY() const;

	float GetMaxHeight() const;

	float GetYOffset() const;

	unsigned int GetUVTiling() const;

	const std::vector<GOLit::Material>& GetMaterials() const;

	GO3D* GetGraphics() const;

	glm::vec3 RayIntersect(const Ray& ray) const;

	bool Loaded() const;

	bool IsWater() const;

private:

	Terrain() = delete;

	Terrain(const Terrain&) = delete;

	Terrain& operator=(const Terrain&) = delete;

	Terrain(Terrain&&) = delete;

	Terrain& operator=(Terrain&&) = delete;

	void CreateAABBs();

public:

private:

	std::vector<std::vector<class AxisAlignedBoundingBox*>> aabbs;

	std::vector<std::vector<glm::vec3>> tileNormals;

	std::string name;

	unsigned int tileX;

	unsigned int tileY;

	unsigned int UVTiling;

	float terrainWidth;

	float terrainHeight;

	float tileWidth;

	float tileHeight;

	float maxHeight;

	float yOffset;

	bool isEnabled;

	std::list<Cell> highlightedCells;

	std::string heightMapPath;

	std::string blendMapName;

	std::atomic<Model*> terrainModel;

	std::atomic<GO3D*> terrainGraphics;

	std::thread createAABBSThread;

	std::atomic<bool> aabbsCreated;

	std::function<void(Model* const)>* modelLoadCallback;

	bool isWater;
};

#endif // TERRAIN_H