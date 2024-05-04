#include "GraphicsObjectTexturedAnimatedLit.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTexturedAnimatedLit.h"

GraphicsObjectTexturedAnimatedLit::GraphicsObjectTexturedAnimatedLit(Model* const model, Texture* const diffuse, Texture* const specular)
{
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(model, diffuse, specular);
}

GraphicsObjectTexturedAnimatedLit::~GraphicsObjectTexturedAnimatedLit()
{
}

void GraphicsObjectTexturedAnimatedLit::SetShine(float shine)
{
	static_cast<GOTexturedAnimatedLit*>(graphics)->SetShine(shine);
}
