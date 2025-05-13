#include "GraphicsObjectColored.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOColored.h"

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

const std::vector<char> GraphicsObjectColored::Serialize() const
{
	return std::vector<char>();
}

void GraphicsObjectColored::Deserialize(const std::vector<char>& data)
{
}
