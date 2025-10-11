#include "TerrainComponent.h"

#include "../../Terrain/Terrain.h"
#include "../../Renderer/Texture/Texture.h"
#include "../../Renderer/Texture/TextureManager.h"
#include "../../Renderer/GraphicsObjects/GOTerrain.h"
#include "../../Math/Shapes/Ray.h"
#include "../../Math/Shapes/LineSegment3D.h"
#include "../../Editor/Editor.h"

TerrainComponent::TerrainComponent() :
	terrain(nullptr)
{
	RegisterComponentClassType<TerrainComponent>(this);

	onEditorEnable = new std::function<void()>([this]()
		{
			if (terrain != nullptr)
			{
				if (!terrain->IsEnabled())
				{
					terrain->ToggleCells();
				}
			}
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);

	onEditorDisable = new std::function<void()>([this]()
		{
			if (terrain != nullptr)
			{
				if (terrain->IsEnabled())
				{
					terrain->ToggleCells();
				}
			}
		});

	Editor::RegisterOnEditorEnable(onEditorDisable);
}

TerrainComponent::TerrainComponent(const std::string& name, const std::string& heightMapPath, const std::vector<GOLit::Material>& heightMaterials, float terrainWidth, float terrainHeight, unsigned int tileX, unsigned int tileY, float maxHeight, float yOffset)
{
	RegisterComponentClassType<TerrainComponent>(this);
	terrain = new Terrain(name, heightMapPath, heightMaterials, terrainWidth, terrainHeight, tileX, tileY, maxHeight, yOffset);
}

TerrainComponent::~TerrainComponent()
{
	if (terrain != nullptr)
	{
		delete terrain;
	}
}

glm::vec3 TerrainComponent::GetTerrainPoint(const glm::vec3& position) const
{
	return terrain->GetTerrainPoint(position);
}

const std::vector<std::vector<AxisAlignedBoundingBox*>>& TerrainComponent::GetCellArray() const
{
	return terrain->GetCellArray();
}

void TerrainComponent::ToggleCells()
{
	terrain->ToggleCells();
}

bool TerrainComponent::ColliderVisible() const
{
	return terrain->IsEnabled();
}

void TerrainComponent::ToggleColliderVisibility()
{
	terrain->ToggleCells();
}

glm::vec3 TerrainComponent::GetLineIntersection(const LineSegment3D& line)
{
	Ray ray(line.GetStart(), glm::normalize(line.GetEnd() - line.GetStart()));

	return terrain->RayIntersect(ray);
}

void TerrainComponent::Update()
{
}

void TerrainComponent::Serialize()
{
	savedStrings["Name"] = terrain->GetName();
	savedStrings["HeightMapPath"] = terrain->GetHeightMapPath();
	savedFloats["Width"] = terrain->GetWidth();
	savedFloats["Height"] = terrain->GetHeight();
	savedInts["TileX"] = terrain->GetTileX();
	savedInts["TileY"] = terrain->GetTileY();
	savedFloats["MaxHeight"] = terrain->GetMaxHeight();
	savedFloats["YOffset"] = terrain->GetYOffset();

	unsigned int matIndex = 0;
	for (const GOLit::Material& mat : terrain->GetMaterials())
	{
		savedStrings["DiffuseTexture" + std::to_string(matIndex)] = mat.diffuseMap->GetName();
		savedStrings["SpecularTexture" + std::to_string(matIndex)] = mat.specularMap->GetName();
		matIndex++;
	}
	
}

void TerrainComponent::Deserialize()
{
	if (terrain != nullptr)
	{
		delete terrain;
	}

	std::vector<GOLit::Material> materials;

	bool allMaterialsLoaded = false;
	unsigned int matIndex = 0;
	while (!allMaterialsLoaded)
	{
		std::string difTextureKey = "DiffuseTexture" + std::to_string(matIndex);
		std::string specTexureKey = "SpecularTexture" + std::to_string(matIndex);

		if (savedStrings.find(difTextureKey) != savedStrings.end())
		{
			materials.push_back(GOLit::Material(TextureManager::GetTexture(savedStrings[difTextureKey]), TextureManager::GetTexture(savedStrings[specTexureKey])));
		}
		else
		{
			allMaterialsLoaded = true;
		}

		matIndex++;
	}

	terrain = new Terrain(savedStrings["Name"], savedStrings["HeightMapPath"], materials, savedFloats["Width"], savedFloats["Height"], savedInts["TileX"], savedInts["TileY"], savedFloats["MaxHeight"], savedFloats["YOffset"]);
}
