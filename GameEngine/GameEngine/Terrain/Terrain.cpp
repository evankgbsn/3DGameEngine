#include "Terrain.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/GraphicsObjects/GraphicsobjectManager.h"
#include "../Renderer/GraphicsObjects/GOTerrain.h"
#include "../Renderer/GraphicsObjects/GOWater.h"
#include "../Renderer/Texture/TextureManager.h"
#include "../Math/Shapes/AxisAlignedBoundingBox.h"
#include "../Math/Math.h"
#include "../Math/Shapes/Triangle.h"

Terrain::Terrain(const std::string& n, const std::string& hmp, const std::vector<GOLit::Material>& heightMaterials, const std::string& blendMap, float w, float h, unsigned int tx, unsigned int ty, float maxHeight, float yo, unsigned int uvTiling, bool iw, bool loadAsync) :
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
	heightMapPath(hmp),
	aabbsCreated(false),
	modelLoadCallback(nullptr),
	isWater(iw),
	UVTiling(uvTiling),
	blendMapName(blendMap)
{
	if (!ModelManager::ModelLoaded(name))
	{
		if (loadAsync)
		{
			modelLoadCallback = new std::function<void(Model* const)>([this, heightMaterials, blendMap](Model* const model)
				{
					terrainModel.store(model);

					if (isWater)
					{
						terrainGraphics.store(GraphicsObjectManager::CreateGOWater(terrainModel));
					}
					else
					{
						terrainGraphics.store(GraphicsObjectManager::CreateGOTerrain(terrainModel, heightMaterials, blendMap));
						static_cast<GOTerrain*>(terrainGraphics.load())->SetShine(8.0f);
					}

					createAABBSThread = std::thread(&Terrain::CreateAABBs, this);
				});

			ModelManager::RegisterCallbackForModelLoaded(name, "TerrainSetup", modelLoadCallback);
			ModelManager::LoadModelTerrain(name, heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset, UVTiling, true);
		}
		else
		{
			if (isWater)
			{
				terrainGraphics.store(GraphicsObjectManager::CreateGOWater(ModelManager::LoadModelTerrain(name, heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset, UVTiling, false)));
			}
			else
			{
				terrainModel.store(ModelManager::LoadModelTerrain(name, heightMapPath, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset, UVTiling, false));
				terrainGraphics.store(GraphicsObjectManager::CreateGOTerrain(terrainModel, heightMaterials, blendMap));
				static_cast<GOTerrain*>(terrainGraphics.load())->SetShine(8.0f);
			}
			
		}
	}
	else
	{
		terrainModel.store(ModelManager::GetModel(name));

		if (isWater)
		{
			terrainGraphics.store(GraphicsObjectManager::CreateGOWater(terrainModel));
		}
		else
		{

			terrainGraphics.store(GraphicsObjectManager::CreateGOTerrain(terrainModel, heightMaterials, blendMap));
			static_cast<GOTerrain*>(terrainGraphics.load())->SetShine(8.0f);
		}

		createAABBSThread = std::thread(&Terrain::CreateAABBs, this);
	}
}

Terrain::~Terrain()
{
	if (modelLoadCallback != nullptr)
	{
		ModelManager::DergisterCallbackForModelLoaded(name, "TerrainSetup");

		delete modelLoadCallback;
	}

	if (aabbsCreated.load())
	{
		for (std::vector<AxisAlignedBoundingBox*>& x : aabbs)
		{
			for (AxisAlignedBoundingBox* y : x)
			{
				delete y;
			}
		}
	}

	if (terrainGraphics.load() != nullptr)
	{
		GraphicsObjectManager::Delete(terrainGraphics);
	}

	if (createAABBSThread.joinable())
	{
		createAABBSThread.join();
	}
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
	if (terrainModel.load() == nullptr || !aabbsCreated.load())
	{
		return glm::vec3();
	}

	Cell cell = TestPoint(point);

	// 4 points of the cell.
	// Get the indices of the four corners of the quad
	// Indices of the quad corners
	int topLeftIdx = (cell.y * tileX) + cell.x;
	int topRightIdx = topLeftIdx + 1;
	int bottomLeftIdx = ((cell.y + 1) * tileX) + cell.x;
	int bottomRightIdx = bottomLeftIdx + 1;

	glm::vec3 a = terrainModel.load()->GetVertices()[bottomLeftIdx].GetPosition();
	glm::vec3 b = terrainModel.load()->GetVertices()[bottomRightIdx].GetPosition();
	glm::vec3 c = terrainModel.load()->GetVertices()[topRightIdx].GetPosition();
	glm::vec3 d = terrainModel.load()->GetVertices()[topLeftIdx].GetPosition();

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
	if (terrainModel.load() == nullptr || !aabbsCreated.load())
	{
		return glm::vec3();
	}

	Cell cell = TestPoint(point);

	// Get the indices of the four corners of the quad
	int topLeftIdx = (cell.y * tileX) + cell.x;
	int topRightIdx = topLeftIdx + 1;
	int bottomLeftIdx = ((cell.y + 1) * tileX) + cell.x;
	int bottomRightIdx = bottomLeftIdx + 1;

	glm::vec3 a = terrainModel.load()->GetVertices()[topLeftIdx].GetPosition();
	glm::vec3 na = terrainModel.load()->GetVertices()[topLeftIdx].GetNormal();
	glm::vec3 b = terrainModel.load()->GetVertices()[topRightIdx].GetPosition();
	glm::vec3 nb = terrainModel.load()->GetVertices()[topRightIdx].GetNormal();
	glm::vec3 c = terrainModel.load()->GetVertices()[bottomLeftIdx].GetPosition();
	glm::vec3 nc = terrainModel.load()->GetVertices()[bottomLeftIdx].GetNormal();
	glm::vec3 d = terrainModel.load()->GetVertices()[bottomRightIdx].GetPosition();
	glm::vec3 nd = terrainModel.load()->GetVertices()[bottomRightIdx].GetNormal();

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

const std::string& Terrain::GetBlendMapName() const
{
	return blendMapName;
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

unsigned int Terrain::GetUVTiling() const
{
	return UVTiling;
}

const std::vector<GOLit::Material>& Terrain::GetMaterials() const
{
	if (terrainGraphics.load() == nullptr || isWater)
	{
		return std::vector<GOLit::Material>();
	}

	return static_cast<GOTerrain*>(terrainGraphics.load())->GetMaterials();
}

GO3D* Terrain::GetGraphics() const
{
	return terrainGraphics;
}

glm::vec3 Terrain::RayIntersect(const Ray& ray) const
{
	if (terrainModel.load() == nullptr || !aabbsCreated.load())
	{
		return glm::vec3();
	}

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
				
				const std::vector<Vertex>& vertices = terrainModel.load()->GetVertices();
				const std::vector<unsigned int>& indices = terrainModel.load()->GetIndices();

				// Get the indices of the four corners of the quad
				int topLeftIdx = (testPoint.y * tileX) + testPoint.x;
				int topRightIdx = topLeftIdx + 1;
				int bottomLeftIdx = ((testPoint.y + 1) * tileX) + testPoint.x;
				int bottomRightIdx = bottomLeftIdx + 1;

				glm::vec3 a = terrainModel.load()->GetVertices()[bottomLeftIdx].GetPosition();
				glm::vec3 b = terrainModel.load()->GetVertices()[bottomRightIdx].GetPosition();
				glm::vec3 c = terrainModel.load()->GetVertices()[topRightIdx].GetPosition();
				glm::vec3 d = terrainModel.load()->GetVertices()[topLeftIdx].GetPosition();

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

bool Terrain::Loaded() const
{
	return aabbsCreated.load();
}

bool Terrain::IsWater() const
{
	return isWater;
}

void Terrain::CreateAABBs()
{
	if (terrainModel.load() == nullptr)
	{
		return;
	}

	const std::vector<Vertex>& vertices = terrainModel.load()->GetVertices();

	// Reserve space to avoid reallocations
	aabbs.reserve((tileX - 1) * (tileY - 1));

	// Loop through each tile (quad) on the terrain
	for (unsigned int z = 0; z < tileY - 1; ++z) {
		aabbs.push_back(std::vector<AxisAlignedBoundingBox*>());
		for (unsigned int x = 0; x < tileX - 1; ++x) {
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

	aabbsCreated.store(true);
}
