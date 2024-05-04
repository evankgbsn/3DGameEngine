#include "GraphicsObjectTexturedLit.h"

#include "../../Renderer/GraphicsObjects/GOTexturedLit.h"
#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"

GraphicsObjectTexturedLit::GraphicsObjectTexturedLit(Model* const model, Texture* const diffuse, Texture* const specular)
{
	graphics = GraphicsObjectManager::CreateGO3DTexturedLit(model, diffuse, specular);
}

GraphicsObjectTexturedLit::~GraphicsObjectTexturedLit()
{
}

void GraphicsObjectTexturedLit::SetShine(float shine)
{
	static_cast<GOTexturedLit*>(graphics)->SetShine(shine);
}
