#include "GraphicsObjectTextured.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTextured.h"

GraphicsObjectTextured::GraphicsObjectTextured(Model* const model, Texture* const texture)
{
	graphics = GraphicsObjectManager::Create3DGOTextured(model, texture);
}

GraphicsObjectTextured::~GraphicsObjectTextured()
{
}


