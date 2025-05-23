#include "OrientedBoundingBoxWithVisualization.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/Model/Vertex.h"


OrientedBoundingBoxWithVisualization::OrientedBoundingBoxWithVisualization(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation) :
	OrientedBoundingBox(initialOrigin, initialSize, initialOrientation)
{
	CreateGraphics();
}

OrientedBoundingBoxWithVisualization::OrientedBoundingBoxWithVisualization(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation) :
	OrientedBoundingBox(vertices, initialOrientation)
{
	CreateGraphics();
}

OrientedBoundingBoxWithVisualization::~OrientedBoundingBoxWithVisualization()
{
	GraphicsObjectManager::Delete(graphics);
}

void OrientedBoundingBoxWithVisualization::Update(const glm::mat4& transform)
{
	glm::mat4 scale(1.0f);
	scale = glm::scale(scale, GetSize());

	glm::mat4 translation(1.0f);
	translation = glm::translate(translation, GetOffset());

	graphics->SetTransform(transform * translation * scale);

	SetOrigin(graphics->GetTranslation());
	SetOrientation(graphics->GetRotation());
}

void OrientedBoundingBoxWithVisualization::ToggleVisibility()
{
	if (graphics->IsDisabled())
	{
		GraphicsObjectManager::Enable(graphics);
	}
	else
	{
		GraphicsObjectManager::Disable(graphics);
	}
}

bool OrientedBoundingBoxWithVisualization::IsVisible() const
{
	return !graphics->IsDisabled();
}

void OrientedBoundingBoxWithVisualization::SetColor(const glm::vec4& newColor)
{
	if (graphics != nullptr)
	{
		graphics->SetColor(newColor);
	}
}

const glm::vec4 OrientedBoundingBoxWithVisualization::GetColor() const
{
	return graphics->GetColor();
}

void OrientedBoundingBoxWithVisualization::CreateGraphics()
{
	if (!ModelManager::ModelLoaded("OrientedBoundingBox"))
	{
		ModelManager::LoadModel("OrientedBoundingBox", "Assets/Model/Cube.gltf");
	}

	graphics = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel("OrientedBoundingBox"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	graphics->SetDrawMode(GO3D::Mode::LINE);
	graphics->SetScale(GetSize());
	graphics->SetTranslation(GetOffset());
	SetOrigin(GetOffset());
}
