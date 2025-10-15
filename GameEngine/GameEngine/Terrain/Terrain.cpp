#include "Terrain.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/GraphicsObjects/GraphicsobjectManager.h"
#include "../Renderer/GraphicsObjects/GOTerrain.h"
#include "../Renderer/Texture/TextureManager.h"
#include "../Math/Shapes/AxisAlignedBoundingBox.h"
#include "../Math/Math.h"
#include "../Math/Shapes/Triangle.h"

Terrain::Terrain(const std::string& n, const std::string& hmp, const std::vector<GOLit::Material>& heightMaterials, float w, float h, unsigned int tx, unsigned int ty, float maxHeight, float yo) :
	aabbs(std::vector<std::vector<AxisAlignedBoundingBox*>>()),
	tileNormals(std::vector<std::vector<glm::vec3>>()),
	terrainWidth(w),
	terrainHeight(h),
	tileX(tx),
	tileY(ty),
	yOffset(yo),
	name(n),
	tileHeight(terrainHeight/tileY),
	tileWidth(terrainWidth/tileX),
	isEnabled(true),
	highlightedCells(std::list<Cell>()),
	heightMapPath(hmp)
{

	ModelManager::CreateModelTerrain(name, heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset);
	terrainModel = ModelManager::GetModel(name);

	terrainGraphics = GraphicsObjectManager::CreateGOTerrain(terrainModel, heightMaterials);
	terrainGraphics->SetShine(8.0f);
	
	CreateAABBs();

	//for (unsigned int x = 0; x < tileX; x++)
	//{
	//	aabbs.push_back(std::vector<AxisAlignedBoundingBoxWithVisualization*>());
	//	for (unsigned int y = 0; y < tileY; y++)
	//	{
	//		// Get the min and max.
	//		glm::vec3 min(INFINITY, INFINITY, INFINITY);
	//		glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);
	//		for (unsigned int i = 0; i < 6; i++)
	//		{
	//			unsigned int index = x * tileY * 6 + y * 6 + i;
	//			min = glm::min(glm::vec3(terrainModel->GetVertices()[index].GetPosition()), min);
	//			max = glm::max(glm::vec3(terrainModel->GetVertices()[index].GetPosition()), max);
	//		}
	//
	//		aabbs[x].push_back(new AxisAlignedBoundingBoxWithVisualization(min, max));
	//	}
	//}

	//ToggleCells();
}

Terrain::~Terrain()
{
	for (std::vector<AxisAlignedBoundingBox*>& x : aabbs)
	{
		for (AxisAlignedBoundingBox* y : x)
		{
			delete y;
		}
	}

	GraphicsObjectManager::Delete(terrainGraphics);
}

bool Terrain::IsEnabled() const
{
	return isEnabled;
}

bool Terrain::IsVisible(const Cell& cell) const
{
	//return aabbs[cell.y][cell.x]->IsVisible();
	return false;
}

void Terrain::ToggleCells()
{
	//if (!isEnabled)
	//{
	//	for (std::vector<AxisAlignedBoundingBoxWithVisualization*>& aabbvector : aabbs)
	//	{
	//		for (AxisAlignedBoundingBoxWithVisualization* aabb : aabbvector)
	//		{
	//			if (!aabb->IsVisible())
	//			{
	//				aabb->ToggleVisibility();
	//			}
	//		}
	//	}
	//}
	//else
	//{
	//	for (std::vector<AxisAlignedBoundingBoxWithVisualization*>& aabbvector : aabbs)
	//	{
	//		for (AxisAlignedBoundingBoxWithVisualization* aabb : aabbvector)
	//		{
	//			if (aabb->IsVisible())
	//			{
	//				aabb->ToggleVisibility();
	//			}
	//		}
	//	}
	//}

	//AxisAlignedBoundingBoxWithVisualization::UpdateInstanceTransforms();

	isEnabled = !isEnabled;
}

void Terrain::ToggleCell(const Cell& cell)
{
	//aabbs[cell.y][cell.x]->ToggleVisibility();
}

void Terrain::HighlightCell(const Cell& cell, const glm::vec4& color)
{
	//if (!aabbs[cell.y][cell.x]->IsVisible())
	//{
	//	aabbs[cell.y][cell.x]->ToggleVisibility();
	//}
	//
	//aabbs[cell.y][cell.x]->SetColor(color);
}

Terrain::Cell Terrain::TestPoint(const glm::vec3& p) const
{
	glm::vec3 point = p;
	point.x += (terrainWidth / 2);
	point.z += (terrainHeight / 2);
	return { static_cast<unsigned int>(glm::clamp(point.x / tileWidth, 0.f, tileX - 2.f)), static_cast<unsigned int>(glm::clamp(point.z / tileHeight,0.f, tileY - 2.f)) };
}

float Terrain::GetCellHeight(const Cell& cell)
{
	float dif = abs(aabbs[cell.x][cell.y]->GetMin().y - aabbs[cell.x][cell.y]->GetMax().y);
	float min = glm::min(aabbs[cell.x][cell.y]->GetMin().y, aabbs[cell.x][cell.y]->GetMax().y);
	return min + dif / 2;
}

glm::vec3 Terrain::GetTerrainPoint(const glm::vec3& point)
{
	Cell cell = TestPoint(point);

	// 4 points of the cell.
	// Get the indices of the four corners of the quad
	// Indices of the quad corners
	int topLeftIdx = (cell.y * tileX) + cell.x;
	int topRightIdx = topLeftIdx + 1;
	int bottomLeftIdx = ((cell.y + 1) * tileX) + cell.x;
	int bottomRightIdx = bottomLeftIdx + 1;

	//glm::vec3 a = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 1].GetPosition();
	//glm::vec3 b = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 2].GetPosition();
	//glm::vec3 c = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 0].GetPosition();
	//glm::vec3 d = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 5].GetPosition();

	glm::vec3 a = terrainModel->GetVertices()[bottomLeftIdx].GetPosition();
	glm::vec3 b = terrainModel->GetVertices()[bottomRightIdx].GetPosition();
	glm::vec3 c = terrainModel->GetVertices()[topRightIdx].GetPosition();
	glm::vec3 d = terrainModel->GetVertices()[topLeftIdx].GetPosition();

	glm::vec3 AToP = point - a;

	// Upper triangle.
	if (-AToP.x > AToP.z)
	{
		Math::Tri tri = { c, d, a };
		Math::BarycentricRelation br = Math::CreateBarycentricRelation(tri, point, tileWidth, true);
		return br.Use(c, d, a);
	}
	// Lower triangle.
	else
	{
		Math::Tri tri = { a, b, c };
		Math::BarycentricRelation br = Math::CreateBarycentricRelation(tri, point, tileWidth, false);
		return br.Use(a, b, c);
	}
}

glm::vec3 Terrain::GetTerrainNormal(const glm::vec3& point)
{
	Cell cell = TestPoint(point);

	// 4 points of the cell.
	//glm::vec3 a = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 1].GetPosition();
	//glm::vec3 na = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 1].GetNormal();
	//glm::vec3 b = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 2].GetPosition();
	//glm::vec3 nb = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 2].GetNormal();
	//glm::vec3 c = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 0].GetPosition();
	//glm::vec3 nc = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 0].GetNormal();
	//glm::vec3 d = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 5].GetPosition();
	//glm::vec3 nd = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 5].GetNormal();

	// Get the indices of the four corners of the quad
	int topLeftIdx = (cell.y * tileX) + cell.x;
	int topRightIdx = topLeftIdx + 1;
	int bottomLeftIdx = ((cell.y + 1) * tileX) + cell.x;
	int bottomRightIdx = bottomLeftIdx + 1;

	glm::vec3 a = terrainModel->GetVertices()[topLeftIdx].GetPosition();
	glm::vec3 na = terrainModel->GetVertices()[topLeftIdx].GetNormal();
	glm::vec3 b = terrainModel->GetVertices()[topRightIdx].GetPosition();
	glm::vec3 nb = terrainModel->GetVertices()[topRightIdx].GetNormal();
	glm::vec3 c = terrainModel->GetVertices()[bottomLeftIdx].GetPosition();
	glm::vec3 nc = terrainModel->GetVertices()[bottomLeftIdx].GetNormal();
	glm::vec3 d = terrainModel->GetVertices()[bottomRightIdx].GetPosition();
	glm::vec3 nd = terrainModel->GetVertices()[bottomRightIdx].GetNormal();

	glm::vec3 AToP = point - a;

	// Upper triangle.
	if (-AToP.x > AToP.z)
	{
		Math::Tri tri = { c, d, a };
		Math::BarycentricRelation br = Math::CreateBarycentricRelation(tri, point, tileWidth, true);
		return br.Use(nc, nd, na);
	}
	// Lower triangle.
	else
	{
		Math::Tri tri = { a, b, c };
		Math::BarycentricRelation br = Math::CreateBarycentricRelation(tri, point, tileWidth, false);
		return br.Use(na, nb, nc);
	}
}

void Terrain::UpdateHeightByTerrainPoint(const glm::vec3& point, float heightChange)
{
	Cell cell = TestPoint(point);

	// 4 points of the cell.
	terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 1].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 2].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 0].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 5].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 3].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 4].GetPosition().y += heightChange;

	terrainModel->GetVertices()[(cell.x + 1) * tileY * 6 + cell.y * 6 + 1].GetPosition().y += heightChange;
	terrainModel->GetVertices()[(cell.x + 1) * tileY * 6 + cell.y * 6 + 5].GetPosition().y += heightChange;
	terrainModel->GetVertices()[(cell.x + 1) * tileY * 6 + cell.y * 6 + 3].GetPosition().y += heightChange;

	terrainModel->GetVertices()[(cell.x - 1) * tileY * 6 + cell.y * 6 + 2].GetPosition().y += heightChange;
	terrainModel->GetVertices()[(cell.x - 1) * tileY * 6 + cell.y * 6 + 0].GetPosition().y += heightChange;
	terrainModel->GetVertices()[(cell.x - 1) * tileY * 6 + cell.y * 6 + 4].GetPosition().y += heightChange;


	terrainModel->GetVertices()[cell.x * tileY * 6 + (cell.y + 1) * 6 + 1].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + (cell.y + 1) * 6 + 2].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + (cell.y + 1) * 6 + 3].GetPosition().y += heightChange;

	terrainModel->GetVertices()[cell.x * tileY * 6 + (cell.y - 1) * 6 + 0].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + (cell.y - 1) * 6 + 5].GetPosition().y += heightChange;
	terrainModel->GetVertices()[cell.x * tileY * 6 + (cell.y - 1) * 6 + 4].GetPosition().y += heightChange;

	terrainModel->GetVertices()[(cell.x + 1) * tileY * 6 + (cell.y + 1) * 6 + 1].GetPosition().y += heightChange;
	terrainModel->GetVertices()[(cell.x + 1) * tileY * 6 + (cell.y + 1) * 6 + 3].GetPosition().y += heightChange;

	terrainModel->GetVertices()[(cell.x + 1) * tileY * 6 + (cell.y - 1) * 6 + 5].GetPosition().y += heightChange;

	terrainModel->GetVertices()[(cell.x - 1) * tileY * 6 + (cell.y - 1) * 6 + 4].GetPosition().y += heightChange;
	terrainModel->GetVertices()[(cell.x - 1) * tileY * 6 + (cell.y - 1) * 6 + 0].GetPosition().y += heightChange;

	terrainModel->GetVertices()[(cell.x - 1) * tileY * 6 + (cell.y + 1) * 6 + 2].GetPosition().y += heightChange;


	terrainModel->UpdateBuffer();


	auto updateCellAABBS = [this, cell](unsigned int xOffset, unsigned int yOffset)
		{
			glm::vec3 cellPoints[6] = {
				terrainModel->GetVertices()[(cell.x + xOffset) * tileY * 6 + (cell.y + yOffset) * 6 + 1].GetPosition(),
				terrainModel->GetVertices()[(cell.x + xOffset) * tileY * 6 + (cell.y + yOffset) * 6 + 2].GetPosition(),
				terrainModel->GetVertices()[(cell.x + xOffset) * tileY * 6 + (cell.y + yOffset) * 6 + 0].GetPosition(),
				terrainModel->GetVertices()[(cell.x + xOffset) * tileY * 6 + (cell.y + yOffset) * 6 + 5].GetPosition(),
				terrainModel->GetVertices()[(cell.x + xOffset) * tileY * 6 + (cell.y + yOffset) * 6 + 3].GetPosition(),
				terrainModel->GetVertices()[(cell.x + xOffset) * tileY * 6 + (cell.y + yOffset) * 6 + 4].GetPosition()
			};

			// Get the min and max.
			glm::vec3 min(INFINITY, INFINITY, INFINITY);
			glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);
			for (unsigned int i = 0; i < 6; i++)
			{
				min = glm::min(glm::vec3(cellPoints[i]), min);
				max = glm::max(glm::vec3(cellPoints[i]), max);
			}

			aabbs[cell.x + xOffset][cell.y + yOffset]->FromMinAndMax(min, max);
			//aabbs[cell.x + xOffset][cell.y + yOffset]->Update();
			//aabbs[cell.x + xOffset][cell.y + yOffset]->UpdateGraphicsInstance();
		};


	updateCellAABBS(0, 0);
	updateCellAABBS(-1, 0);
	updateCellAABBS(-1, -1);
	updateCellAABBS(-1, 1);
	updateCellAABBS(1, -1);
	updateCellAABBS(1, 0);
	updateCellAABBS(1, 1);
	updateCellAABBS(0, 1);
	updateCellAABBS(0, -1);

}

void Terrain::VisualizeAllCells()
{
	//for (auto& x : aabbs)
	//{
	//	for (auto& y : x)
	//	{
	//		if (true)
	//		{
	//			if (!y->IsVisible())
	//			{
	//				y->ToggleVisibility();
	//			}
	//		}
	//	}
	//}

	//AxisAlignedBoundingBoxWithVisualization::UpdateInstanceTransforms();
}

const std::vector<std::vector<class AxisAlignedBoundingBox*>>& Terrain::GetCellArray() const
{
	return aabbs;
}

void Terrain::UpdateTerrainCells()
{
}

const std::string& Terrain::GetHeightMapPath() const
{
	return heightMapPath;
}

const std::string& Terrain::GetName() const
{
	return name;
}

float Terrain::GetWidth() const
{
	return terrainWidth;
}

float Terrain::GetHeight() const
{
	return terrainHeight;
}

unsigned int Terrain::GetTileX() const
{
	return tileX;
}

unsigned int Terrain::GetTileY() const
{
	return tileY;
}

float Terrain::GetMaxHeight() const
{
	return maxHeight;
}

float Terrain::GetYOffset() const
{
	return yOffset;
}

const std::vector<GOLit::Material>& Terrain::GetMaterials() const
{
	return terrainGraphics->GetMaterials();
}

GOTerrain* Terrain::GetGraphics() const
{
	return terrainGraphics;
}

glm::vec3 Terrain::RayIntersect(const Ray& ray) const
{
	// 1. INITIALIZATION

	std::vector<Cell> traversedCells;

	// The grid cell we are currently in (integer coordinates)
	Cell testPoint = TestPoint(ray.GetOrigin());

	// The distance the ray travels to cross one full grid cell (horizontally or vertically)
	// Avoid division by zero if the ray is parallel to an axis.
	double deltaDistX = (ray.GetDirection().x == 0) ? 1e30 : std::abs(tileWidth / ray.GetDirection().x);
	double deltaDistZ = (ray.GetDirection().z == 0) ? 1e30 : std::abs(tileHeight / ray.GetDirection().z);

	// The distance from the ray's origin to the next grid line
	double sideDistX;
	double sideDistZ;

	// The direction to step in on each axis (+1 or -1)
	int stepX;
	int stepZ;

	if (ray.GetDirection().x < 0) {
		stepX = -1;
		sideDistX = (((ray.GetOrigin().x + (terrainWidth / 2)) / tileWidth) - testPoint.x) * deltaDistX;
	}
	else {
		stepX = 1;
		sideDistX = (testPoint.x + 1.0 - ((ray.GetOrigin().x + (terrainWidth / 2)) / tileWidth)) * deltaDistX;
	}

	if (ray.GetDirection().z < 0) {
		stepZ = -1;
		sideDistZ = (((ray.GetOrigin().z + (terrainHeight / 2)) / tileHeight) - testPoint.y) * deltaDistZ;
	}
	else {
		stepZ = 1;
		sideDistZ = (testPoint.y + 1.0 - ((ray.GetOrigin().z + (terrainHeight / 2)) / tileHeight)) * deltaDistZ;
	}

	// 2. THE MARCHING LOOP
	int marchSteps = 0;
	const int maxMarchSteps = tileX * tileY; // Safety limit

	while (marchSteps < maxMarchSteps) {
		// Check if we are inside the terrain boundaries
		if (testPoint.x >= 0 && testPoint.x <  tileX && testPoint.y >= 0 && testPoint.y < tileY) {
			
			traversedCells.push_back(testPoint);
			
			// --- THIS IS WHERE YOU WOULD TEST FOR INTERSECTION ---
			// AABB test: rayIntersectsAABB(ray, getAABBForTile(mapX, mapZ))
			// If it hits, do the precise ray-triangle tests for this cell.
			// If you find an intersection, you can break the loop.


			if (aabbs[testPoint.y][testPoint.x]->RayIntersect(ray) != -1.0f)
			{
				
				const std::vector<Vertex>& vertices = terrainModel->GetVertices();
				const std::vector<unsigned int>& indices = terrainModel->GetIndices();

				// Get the indices of the four corners of the quad
				int topLeftIdx = (testPoint.y * tileX) + testPoint.x;
				int topRightIdx = topLeftIdx + 1;
				int bottomLeftIdx = ((testPoint.y + 1) * tileX) + testPoint.x;
				int bottomRightIdx = bottomLeftIdx + 1;

				glm::vec3 a = terrainModel->GetVertices()[bottomLeftIdx].GetPosition();
				glm::vec3 b = terrainModel->GetVertices()[bottomRightIdx].GetPosition();
				glm::vec3 c = terrainModel->GetVertices()[topRightIdx].GetPosition();
				glm::vec3 d = terrainModel->GetVertices()[topLeftIdx].GetPosition();

				Triangle t1 = { a, b, c };
				Triangle t2 = { c, d, a };

				float hit1 = t1.Raycast(ray);
				float hit2 = t2.Raycast(ray);


				if (hit1 != -1.0f)
				{
					//aabbs[testPoint.y][testPoint.x]->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
					return ray.GetOrigin() + ray.GetDirection() * hit1;
				}
				else if (hit2 != -1.0f)
				{
					//aabbs[testPoint.y][testPoint.x]->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
					return ray.GetOrigin() + ray.GetDirection() * hit2;
				}
			}
		}

		// Advance to the next grid cell
		if (sideDistX < sideDistZ) {
			sideDistX += deltaDistX;
			testPoint.x += stepX;
		}
		else {
			sideDistZ += deltaDistZ;
			testPoint.y += stepZ;
		}

		// Check if we've left the terrain after stepping
		if (testPoint.x < 0 || testPoint.x >= tileX - 1 || testPoint.y < 0 || testPoint.y >= tileY - 1) {
			break; // Ray has exited the grid
		}

		

		marchSteps++;
	}

	return glm::vec3(0.0f);
}

void Terrain::CreateAABBs()
{
	const std::vector<Vertex>& vertices = terrainModel->GetVertices();

	// Reserve space to avoid reallocations
	aabbs.reserve((tileX - 1) * (tileY - 1));

	// Loop through each tile (quad) on the terrain
	for (int z = 0; z < tileY - 1; ++z) {
		aabbs.push_back(std::vector<AxisAlignedBoundingBox*>());
		for (int x = 0; x < tileX - 1; ++x) {
			// Get the indices of the four corners of the quad
			int topLeftIdx = (z * tileX) + x;
			int topRightIdx = topLeftIdx + 1;
			int bottomLeftIdx = ((z + 1) * tileX) + x;
			int bottomRightIdx = bottomLeftIdx + 1;

			// Get the Y-positions of the four vertices
			float y_tl = vertices[topLeftIdx].GetPosition().y;
			float y_tr = vertices[topRightIdx].GetPosition().y;
			float y_bl = vertices[bottomLeftIdx].GetPosition().y;
			float y_br = vertices[bottomRightIdx].GetPosition().y;

			// Find the minimum and maximum height for this tile
			float minY = std::min({ y_tl, y_tr, y_bl, y_br });
			float maxY = std::max({ y_tl, y_tr, y_bl, y_br });

			float xPos = static_cast<float>(x) * (terrainWidth / tileX) - (terrainWidth / 2);
			float xPos1 = (static_cast<float>(x) + 1.0f) * (terrainWidth / tileX) - (terrainWidth / 2);
			float zPos = static_cast<float>(z) * (terrainHeight / tileY) - (terrainHeight / 2);
			float zPos1 = (static_cast<float>(z) + 1.0f) * (terrainHeight / tileY) - (terrainHeight / 2);

			aabbs[z].push_back(new AxisAlignedBoundingBox(glm::vec3(xPos, minY, zPos) , glm::vec3(xPos1, maxY, zPos1)));
		}
	}
}
