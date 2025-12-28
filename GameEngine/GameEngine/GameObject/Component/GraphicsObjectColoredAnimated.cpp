#include "GraphicsObjectColoredAnimated.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOColoredAnimated.h"
#include "../../Renderer/Model/Model.h"
#include "../../Renderer/Model/ModelManager.h"

GraphicsObjectColoredAnimated::GraphicsObjectColoredAnimated()
{
	RegisterComponentClassType<GraphicsObjectColoredAnimated>(this);
}

GraphicsObjectColoredAnimated::GraphicsObjectColoredAnimated(Model* const model, const glm::vec4& color)
{
	RegisterComponentClassType<GraphicsObjectColoredAnimated>(this);
	graphics = GraphicsObjectManager::CreateGO3DColoredAnimated(model, color);
}

GraphicsObjectColoredAnimated::~GraphicsObjectColoredAnimated()
{
}

const glm::mat4* const GraphicsObjectColoredAnimated::GetAnimPoseArray() const
{
	return static_cast<GOColoredAnimated*>(graphics)->GetAnimPoseArray();
}

const std::vector<glm::mat4>& GraphicsObjectColoredAnimated::GetAnimInvBindPoseArray() const
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

void GraphicsObjectColoredAnimated::Serialize()
{
	GraphicsObject3DComponent::Serialize();

	GOColoredAnimated* go = static_cast<GOColoredAnimated*>(graphics);

	savedVec3s["Color"] = go->GetColor();
	savedFloats["AnimationSpeed"] = go->GetSpeed();
	savedInts["AnimationClip"] = go->GetClip();
	savedInts["AnimationFrame"] = go->GetFrame();
	savedStrings["ModelName"] = GetModel()->GetName();

}

void GraphicsObjectColoredAnimated::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DColoredAnimated(ModelManager::GetModel(savedStrings["ModelName"]), glm::vec4(savedVec3s["Color"], 1.0f));

	GOColoredAnimated* go = static_cast<GOColoredAnimated*>(graphics);

	go->SetColor(glm::vec4(savedVec3s["Color"], 1.0f));
	go->SetClip(savedInts["AnimationClip"]);
	go->SetFrame(savedInts["AnimationFrame"]);
	go->SetSpeed(savedFloats["AnimationSpeed"]);
	GraphicsObject3DComponent::Deserialize();
}
