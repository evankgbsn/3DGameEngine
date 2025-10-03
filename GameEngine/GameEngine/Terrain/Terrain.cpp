#include "Terrain.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/GraphicsObjects/GraphicsobjectManager.h"
#include "../Renderer/GraphicsObjects/GOTerrain.h"
#include "../Renderer/Texture/TextureManager.h"
#include "../Collision/AxisAlignedBoundingBoxWithVisualization.h"
#include "../Math/Math.h"

Terrain::Terrain(const std::string& n, const std::string& hmp, const std::vector<GOLit::Material>& heightMaterials, float w, float h, unsigned int tx, unsigned int ty, float maxHeight, float yo) :
	aabbs(std::vector<std::vector<AxisAlignedBoundingBoxWithVisualization*>>()),
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
	
	for (unsigned int x = 0; x < tileX; x++)
	{
		aabbs.push_back(std::vector<AxisAlignedBoundingBoxWithVisualization*>());
		for (unsigned int y = 0; y < tileY; y++)
		{
			// Get the min and max.
			glm::vec3 min(INFINITY, INFINITY, INFINITY);
			glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);
			for (unsigned int i = 0; i < 6; i++)
			{
				unsigned int index = x * tileY * 6 + y * 6 + i;
				min = glm::min(glm::vec3(terrainModel->GetVertices()[index].GetPosition()), min);
				max = glm::max(glm::vec3(terrainModel->GetVertices()[index].GetPosition()), max);
			}

			aabbs[x].push_back(new AxisAlignedBoundingBoxWithVisualization(min, max));
		}
	}

	AxisAlignedBoundingBoxWithVisualization::UpdateInstanceTransforms();

	ToggleCells();
}

Terrain::~Terrain()
{
	for (std::vector<AxisAlignedBoundingBoxWithVisualization*>& x : aabbs)
	{
		for (AxisAlignedBoundingBox* y : x)
		{
			delete y;
		}
	}

	GraphicsObjectManager::Delete(terrainGraphics);
}

void Terrain::ToggleCells()
{
	if (isEnabled)
	{
		for (std::vector<AxisAlignedBoundingBoxWithVisualization*>& aabbvector : aabbs)
		{
			for (AxisAlignedBoundingBoxWithVisualization* aabb : aabbvector)
			{
				aabb->ToggleVisibility();
			}
		}
	}
}

void Terrain::HighlightCell(const Cell& cell)
{
	
}

Terrain::Cell Terrain::TestPoint(const glm::vec3& p)
{
	glm::vec3 point = p;
	point.x += (terrainWidth / 2);
	point.z -= (terrainHeight / 2);
	return { static_cast<unsigned int>(glm::clamp(point.x / tileWidth, 0.f, tileX - 1.f)), static_cast<unsigned int>(glm::clamp(point.z / -tileHeight,0.f, tileY - 1.f)) };
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
	glm::vec3 a = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 1].GetPosition();
	glm::vec3 b = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 2].GetPosition();
	glm::vec3 c = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 0].GetPosition();
	glm::vec3 d = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 5].GetPosition();

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
	glm::vec3 a = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 1].GetPosition();
	glm::vec3 na = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 1].GetNormal();
	glm::vec3 b = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 2].GetPosition();
	glm::vec3 nb = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 2].GetNormal();
	glm::vec3 c = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 0].GetPosition();
	glm::vec3 nc = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 0].GetNormal();
	glm::vec3 d = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 5].GetPosition();
	glm::vec3 nd = terrainModel->GetVertices()[cell.x * tileY * 6 + cell.y * 6 + 5].GetNormal();

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
			aabbs[cell.x + xOffset][cell.y + yOffset]->Update();
			aabbs[cell.x + xOffset][cell.y + yOffset]->UpdateGraphicsInstance();
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
}

const std::vector<std::vector<class AxisAlignedBoundingBoxWithVisualization*>>& Terrain::GetCellArray() const
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
