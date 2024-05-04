#include "GraphicsObjectTexturedAnimated.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTexturedAnimated.h"

GraphicsObjectTexturedAnimated::GraphicsObjectTexturedAnimated(Model* const model, Texture* const texture)
{
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimated(model, texture);
}

GraphicsObjectTexturedAnimated::~GraphicsObjectTexturedAnimated()
{
}

glm::mat4* GraphicsObjectTexturedAnimated::GetAnimPoseArray()
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetAnimPoseArray();
}

glm::mat4* GraphicsObjectTexturedAnimated::GetAnimInvBindPoseArray()
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetAnimInvBindPoseArray();
}

void GraphicsObjectTexturedAnimated::SetClip(unsigned int clipIndex)
{
	static_cast<GOTexturedAnimated*>(graphics)->SetClip(clipIndex);
}

unsigned int GraphicsObjectTexturedAnimated::GetClip() const
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetClip();
}

unsigned int GraphicsObjectTexturedAnimated::GetFrame() const
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetFrame();
}

void GraphicsObjectTexturedAnimated::SetFrame(unsigned int frameIndex)
{
	static_cast<GOTexturedAnimated*>(graphics)->SetFrame(frameIndex);
}

float GraphicsObjectTexturedAnimated::GetSpeed() const
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetSpeed();
}

void GraphicsObjectTexturedAnimated::SetSpeed(float speed)
{
	static_cast<GOTexturedAnimated*>(graphics)->SetSpeed(speed);
}
