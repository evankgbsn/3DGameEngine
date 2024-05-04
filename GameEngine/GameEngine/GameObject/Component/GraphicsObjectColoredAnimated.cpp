#include "GraphicsObjectColoredAnimated.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOColoredAnimated.h"

GraphicsObjectColoredAnimated::GraphicsObjectColoredAnimated(Model* const model, const glm::vec4& color)
{
	graphics = GraphicsObjectManager::CreateGO3DColoredAnimated(model, color);
}

GraphicsObjectColoredAnimated::~GraphicsObjectColoredAnimated()
{
}

glm::mat4* GraphicsObjectColoredAnimated::GetAnimPoseArray()
{
	return static_cast<GOColoredAnimated*>(graphics)->GetAnimPoseArray();
}

glm::mat4* GraphicsObjectColoredAnimated::GetAnimInvBindPoseArray()
{
	return static_cast<GOColoredAnimated*>(graphics)->GetAnimInvBindPoseArray();
}

void GraphicsObjectColoredAnimated::SetClip(unsigned int clipIndex)
{
	static_cast<GOColoredAnimated*>(graphics)->SetClip(clipIndex);
}

unsigned int GraphicsObjectColoredAnimated::GetClip() const
{
	return static_cast<GOColoredAnimated*>(graphics)->GetClip();
}

unsigned int GraphicsObjectColoredAnimated::GetFrame() const
{
	return static_cast<GOColoredAnimated*>(graphics)->GetFrame();
}

void GraphicsObjectColoredAnimated::SetFrame(unsigned int frameIndex)
{
	static_cast<GOColoredAnimated*>(graphics)->SetFrame(frameIndex);
}

float GraphicsObjectColoredAnimated::GetSpeed() const
{
	return static_cast<GOColoredAnimated*>(graphics)->GetSpeed();
}

void GraphicsObjectColoredAnimated::SetSpeed(float speed)
{
	static_cast<GOColoredAnimated*>(graphics)->SetSpeed(speed);
}
