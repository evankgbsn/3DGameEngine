#include "GraphicsObjectColored.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOColored.h"
#include "../../Renderer/Model/Model.h"
#include "../../Renderer/Model/ModelManager.h"

GraphicsObjectColored::GraphicsObjectColored(Model* const model, const glm::vec4& color)
{
	RegisterComponentClassType<GraphicsObjectColored>(this);
	graphics = GraphicsObjectManager::CreateGO3DColored(model, color);
}

GraphicsObjectColored::~GraphicsObjectColored()
{
	
}

void GraphicsObjectColored::SetColor(const glm::vec4& newColor)
{
	static_cast<GOColored*>(graphics)->SetColor(newColor);
}

const glm::vec4& GraphicsObjectColored::GetColor() const
{
	return static_cast<GOColored*>(graphics)->GetColor();
}

void GraphicsObjectColored::Serialize()
{
	GraphicsObject3DComponent::Serialize();

	savedVec3s["Color"] = GetColor();
	savedStrings["ModelName"] = GetModel()->GetName();
}

void GraphicsObjectColored::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DColored(ModelManager::GetModel(savedStrings["ModelName"]), glm::vec4(savedVec3s["Color"], 1.0f));

	GraphicsObject3DComponent::Deserialize();
}
