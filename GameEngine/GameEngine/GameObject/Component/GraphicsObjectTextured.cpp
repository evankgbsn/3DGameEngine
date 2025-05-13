#include "GraphicsObjectTextured.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTextured.h"

GraphicsObjectTextured::GraphicsObjectTextured(Model* const model, Texture* const texture)
{
	RegisterComponentClassType<GraphicsObjectTextured>(this);
	graphics = GraphicsObjectManager::Create3DGOTextured(model, texture);
}

GraphicsObjectTextured::~GraphicsObjectTextured()
{
}

const std::vector<char> GraphicsObjectTextured::Serialize() const
{
	return std::vector<char>();
}

void GraphicsObjectTextured::Deserialize(const std::vector<char>& data)
{
}


