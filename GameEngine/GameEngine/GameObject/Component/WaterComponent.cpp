#include "WaterComponent.h"

#include "../Renderer/GraphicsObjects/GOWater.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Terrain/Terrain.h"
#include "../Editor/Editor.h"
#include "../Math/Shapes/LineSegment3D.h"

WaterComponent::WaterComponent()
{
	RegisterComponentClassType<WaterComponent>(this);

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

WaterComponent::WaterComponent(const std::string& name, float width, float height, unsigned int tileX, unsigned int tileY, float yOffset)
{
	RegisterComponentClassType<WaterComponent>(this);
	terrain = new Terrain(name, "Assets/Texture/Black.png", {}, "", width, height, tileX, tileY, 0.0f, yOffset, 1U, true);
}

WaterComponent::~WaterComponent()
{
}

glm::vec3 WaterComponent::GetTerrainPoint(const glm::vec3& position) const
{
	return terrain->GetTerrainPoint(position);
}

const std::vector<std::vector<AxisAlignedBoundingBox*>>& WaterComponent::GetCellArray() const
{
	return terrain->GetCellArray();
}

void WaterComponent::ToggleCells()
{
	terrain->ToggleCells();
}

bool WaterComponent::ColliderVisible() const
{
	return terrain->IsEnabled();
}

void WaterComponent::ToggleColliderVisibility()
{
	terrain->ToggleCells();
}

glm::vec3 WaterComponent::GetLineIntersection(const LineSegment3D& line)
{
	Ray ray(line.GetStart(), glm::normalize(line.GetEnd() - line.GetStart()));

	return terrain->RayIntersect(ray);
}

bool WaterComponent::Loaded() const
{
	return terrain->Loaded();
}


void WaterComponent::Update()
{
}

void WaterComponent::Serialize()
{
	savedStrings["Name"] = terrain->GetName();
	savedStrings["HeightMapPath"] = terrain->GetHeightMapPath();
	savedFloats["Width"] = terrain->GetWidth();
	savedFloats["Height"] = terrain->GetHeight();
	savedInts["TileX"] = terrain->GetTileX();
	savedInts["TileY"] = terrain->GetTileY();
	savedFloats["MaxHeight"] = terrain->GetMaxHeight();
	savedFloats["YOffset"] = terrain->GetYOffset();
}

void WaterComponent::Deserialize()
{
	if (terrain != nullptr)
	{
		delete terrain;
	}

	terrain = new Terrain(savedStrings["Name"], savedStrings["HeightMapPath"], {}, "", savedFloats["Width"], savedFloats["Height"], savedInts["TileX"], savedInts["TileY"], savedFloats["MaxHeight"], savedFloats["YOffset"], 1U, true);
}
