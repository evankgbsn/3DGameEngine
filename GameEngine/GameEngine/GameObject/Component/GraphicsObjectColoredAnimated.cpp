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

void GraphicsObjectColoredAnimated::SetClip(const std::string& clipName)
{
	static_cast<GOColoredAnimated*>(graphics)->SetClip(clipName);
}

const std::string& GraphicsObjectColoredAnimated::GetClip() const
{
	return static_cast<GOColoredAnimated*>(graphics)->GetClipName();
}

float GraphicsObjectColoredAnimated::GetSampleTime() const
{
	return static_cast<GOColoredAnimated*>(graphics)->GetSampleTime();
}

void GraphicsObjectColoredAnimated::SetSampleTime(float sampleTime)
{
	static_cast<GOColoredAnimated*>(graphics)->SetSampleTime(sampleTime);
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
	savedStrings["AnimationClip"] = go->GetClipName();
	savedInts["AnimationFrame"] = go->GetSampleTime();
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
	go->SetClip(savedStrings["AnimationClip"]);
	go->SetSampleTime(savedFloats["AnimationFrame"]);
	go->SetSpeed(savedFloats["AnimationSpeed"]);
	GraphicsObject3DComponent::Deserialize();
}
