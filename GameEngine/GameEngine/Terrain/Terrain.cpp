#include "Terrain.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/GraphicsObjects/GraphicsobjectManager.h"
#include "../Renderer/GraphicsObjects/GOTerrain.h"
#include "../Renderer/Texture/TextureManager.h"
#include "../Collision/AxisAlignedBoundingBoxWithVisualization.h"
#include "../Math/Math.h"

Terrain::Terrain(const std::string& n, const std::string& heightMapPath, const std::string& diffTextureName, const std::string& specTextureName, float w, float h, unsigned int tx, unsigned int ty, float maxHeight, float yo) :
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
	highlightedCells(std::list<Cell>())
{

	ModelManager::CreateModelTerrain(name, heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset);
	terrainModel = ModelManager::GetModel(name);

	if (!TextureManager::TextureLoaded("Grass"))
	{
		TextureManager::LoadTexture("Assets/Texture/Green.png", "Grass");
	}

	terrainGraphics = GraphicsObjectManager::CreateGOTerrain(terrainModel, std::vector<GOLit::Material>({ GOLit::Material(TextureManager::GetTexture(diffTextureName), TextureManager::GetTexture(specTextureName)), GOLit::Material(TextureManager::GetTexture("Grass"), TextureManager::GetTexture("Grass"))}));
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

void Terrain::HighlightCell(Cell cell)
{
	
}

Terrain::Cell Terrain::TestPoint(glm::vec3 point)
{
	point.x += (terrainWidth / 2);
	point.z -= (terrainHeight / 2);
	return { static_cast<unsigned int>(glm::clamp(point.x / tileWidth, 0.f, tileX - 1.f)), static_cast<unsigned int>(glm::clamp(point.z / -tileHeight,0.f, tileY - 1.f)) };
}

float Terrain::GetCellHeight(Cell cell)
{
	float dif = abs(aabbs[cell.x][cell.y]->GetMin().y - aabbs[cell.x][cell.y]->GetMax().y);
	float min = glm::min(aabbs[cell.x][cell.y]->GetMin().y, aabbs[cell.x][cell.y]->GetMax().y);
	return min + dif / 2;
}

glm::vec3 Terrain::GetTerrainPoint(glm::vec3 point)
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

glm::vec3 Terrain::GetTerrainNormal(glm::vec3 point)
{
	return glm::vec3();
}

void Terrain::VisualizeAllCells()
{
}

const std::vector<std::vector<class AxisAlignedBoundingBoxWithVisualization*>>& Terrain::GetCellArray() const
{
	return aabbs;
}
