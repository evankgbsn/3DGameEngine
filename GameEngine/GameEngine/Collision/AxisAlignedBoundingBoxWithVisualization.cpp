#include "AxisAlignedBoundingBoxWithVisualization.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/GOCOloredInstanced.h"

unsigned int AxisAlignedBoundingBoxWithVisualization::instanceIDGenerator = 0;
GOColoredInstanced* AxisAlignedBoundingBoxWithVisualization::graphics = nullptr;

AxisAlignedBoundingBoxWithVisualization::AxisAlignedBoundingBoxWithVisualization(const glm::vec3& initialMin, const glm::vec3& initialMax) :
	instanceID(instanceIDGenerator++)
{
	FromMinAndMax(initialMin, initialMax);

	if (graphics == nullptr)
	{
		if (!ModelManager::ModelLoaded("OrientedBoundingBox"))
		{
			ModelManager::LoadModel("OrientedBoundingBox", "Assets/Model/Cube.gltf");
		}

		graphics = GraphicsObjectManager::CreateGO3DColoredInstanced(ModelManager::GetModel("OrientedBoundingBox"), { 0.0f, 0.0f, 1.0f, 1.0f }, 1);
	}
	else
	{
		graphics->AddInstance();
	}

	CreateGraphics();
}

void AxisAlignedBoundingBoxWithVisualization::FromOriginAndSize(const glm::vec3& o, const glm::vec3& s)
{
	origin = o;
	size = s;
}

AxisAlignedBoundingBoxWithVisualization::~AxisAlignedBoundingBoxWithVisualization()
{
	GraphicsObjectManager::Delete(graphics);
	graphics = nullptr;
	instanceIDGenerator = 0;
}

void AxisAlignedBoundingBoxWithVisualization::Update()
{
	graphics->SetScale(GetSize(), instanceID);
	graphics->SetTranslation(GetOrigin(), instanceID);
}

void AxisAlignedBoundingBoxWithVisualization::ToggleVisibility()
{
	if (!graphics->IsDisabled())
	{
		//GraphicsObjectManager::Disable(graphics);
	}
	else
	{
		GraphicsObjectManager::Enable(graphics);
	}
}

void AxisAlignedBoundingBoxWithVisualization::SetColor(const glm::vec4& newColor)
{
	graphics->SetColor(newColor, instanceID);
}

const glm::vec4 AxisAlignedBoundingBoxWithVisualization::GetColor() const
{
	return graphics->GetColor(instanceID);
}

void AxisAlignedBoundingBoxWithVisualization::UpdateInstanceTransforms()
{
	graphics->FinalizeTransforms();
}

void AxisAlignedBoundingBoxWithVisualization::UpdateGraphicsInstance()
{
	graphics->UpdateInstanceByID(instanceID);
}

void AxisAlignedBoundingBoxWithVisualization::CreateGraphics()
{
	graphics->SetScale(GetSize(), instanceID);
	graphics->SetTranslation(GetOrigin(), instanceID);
}
