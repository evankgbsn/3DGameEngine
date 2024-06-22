#include "AxisAlignedBoundingBoxWithVisualization.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/GOCOlored.h"

AxisAlignedBoundingBoxWithVisualization::AxisAlignedBoundingBoxWithVisualization(const glm::vec3& initialMin, const glm::vec3& initialMax)
{
	FromMinAndMax(initialMin, initialMax);
	CreateGraphics();
	ToggleVisibility();
}

void AxisAlignedBoundingBoxWithVisualization::FromOriginAndSize(const glm::vec3& o, const glm::vec3& s)
{
	origin = o;
	size = s;
}

AxisAlignedBoundingBoxWithVisualization::~AxisAlignedBoundingBoxWithVisualization()
{
	GraphicsObjectManager::Delete(graphics);
}

void AxisAlignedBoundingBoxWithVisualization::Update()
{
	graphics->SetScale(GetSize());
	graphics->SetTranslation(GetOrigin());
}

void AxisAlignedBoundingBoxWithVisualization::ToggleVisibility()
{
	if (!graphics->IsDisabled())
	{
		GraphicsObjectManager::Disable(graphics);
	}
	else
	{
		GraphicsObjectManager::Enable(graphics);
	}
}

void AxisAlignedBoundingBoxWithVisualization::CreateGraphics()
{
	if (!ModelManager::ModelLoaded("OrientedBoundingBox"))
	{
		ModelManager::LoadModel("OrientedBoundingBox", "Assets/Model/Cube.gltf");
	}
	
	graphics = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel("OrientedBoundingBox"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	graphics->SetDrawMode(GO3D::Mode::LINE);
	graphics->SetScale(GetSize());
	graphics->SetTranslation(GetOrigin());
}
