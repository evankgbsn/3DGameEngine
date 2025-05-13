#include "GraphicsObjectTexturedAnimatedLit.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTexturedAnimatedLit.h"

GraphicsObjectTexturedAnimatedLit::GraphicsObjectTexturedAnimatedLit(Model* const model, Texture* const diffuse, Texture* const specular)
{
	RegisterComponentClassType<GraphicsObjectTexturedAnimatedLit>(this);
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(model, diffuse, specular);
}

GraphicsObjectTexturedAnimatedLit::~GraphicsObjectTexturedAnimatedLit()
{
}

void GraphicsObjectTexturedAnimatedLit::SetShine(float shine)
{
	static_cast<GOTexturedAnimatedLit*>(graphics)->SetShine(shine);
}

void GraphicsObjectTexturedAnimatedLit::SetClip(unsigned int clipNum)
{
	static_cast<GOTexturedAnimatedLit*>(graphics)->SetClip(clipNum);
}

void GraphicsObjectTexturedAnimatedLit::SetSpeed(float speed)
{
	static_cast<GOTexturedAnimatedLit*>(graphics)->SetSpeed(speed);
}

const std::vector<char> GraphicsObjectTexturedAnimatedLit::Serialize() const
{
	return std::vector<char>();
}

void GraphicsObjectTexturedAnimatedLit::Deserialize(const std::vector<char>& data)
{
}
