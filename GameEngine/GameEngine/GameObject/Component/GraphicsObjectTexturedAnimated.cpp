#include "GraphicsObjectTexturedAnimated.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTexturedAnimated.h"
#include "../../Renderer/Model/Model.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Texture/Texture.h"
#include "../../Renderer/Texture/TextureManager.h"

GraphicsObjectTexturedAnimated::GraphicsObjectTexturedAnimated(Model* const model, Texture* const texture)
{
	RegisterComponentClassType<GraphicsObjectTexturedAnimated>(this);
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimated(model, texture);
}

GraphicsObjectTexturedAnimated::~GraphicsObjectTexturedAnimated()
{
}

const glm::mat4* const GraphicsObjectTexturedAnimated::GetAnimPoseArray() const
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetAnimPoseArray();
}

const std::vector<glm::mat4>& GraphicsObjectTexturedAnimated::GetAnimInvBindPoseArray()
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetAnimInvBindPoseArray();
}

void GraphicsObjectTexturedAnimated::SetClip(const std::string& clipName)
{
	static_cast<GOTexturedAnimated*>(graphics)->SetClip(clipName);
}

const std::string& GraphicsObjectTexturedAnimated::GetClip() const
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetClipName();
}

float GraphicsObjectTexturedAnimated::GetSampleTime() const
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetSampleTime();
}

void GraphicsObjectTexturedAnimated::SetSampleTime(float sampleTime)
{
	static_cast<GOTexturedAnimated*>(graphics)->SetSampleTime(sampleTime);
}

float GraphicsObjectTexturedAnimated::GetSpeed() const
{
	return static_cast<GOTexturedAnimated*>(graphics)->GetSpeed();
}

void GraphicsObjectTexturedAnimated::SetSpeed(float speed)
{
	static_cast<GOTexturedAnimated*>(graphics)->SetSpeed(speed);
}

bool GraphicsObjectTexturedAnimated::isFading() const
{
	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	return go->isFading();
}

float GraphicsObjectTexturedAnimated::GetFadeToTime() const
{
	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	return go->GetFadeToTime();
}

const std::string& GraphicsObjectTexturedAnimated::GetFadeToClipName() const
{
	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	return go->GetFadeToClipName();
}

void GraphicsObjectTexturedAnimated::RegisterAnimationStartCallback(const std::string& name, std::function<void(const std::string&)>* callback)
{
	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	go->RegisterAnimationStartCallback(name, callback);
}

void GraphicsObjectTexturedAnimated::DeregisterAnimationStartCallback(const std::string& name)
{
	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	go->DeregisterAnimationStartCallback(name);
}

void GraphicsObjectTexturedAnimated::RegisterAnimationStopCallback(const std::string& name, std::function<void(const std::string&)>* callback)
{
	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	go->RegisterAnimationStopCallback(name, callback);
}

void GraphicsObjectTexturedAnimated::DeregisterAnimationStopCallback(const std::string& name)
{
	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	go->DeregisterAnimationStopCallback(name);
}

void GraphicsObjectTexturedAnimated::Serialize()
{
	GraphicsObject3DComponent::Serialize();

	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	savedStrings["ModelName"] = GetModel()->GetName();
	savedStrings["TextureName"] = go->GetTexture()->GetName();
	savedFloats["AnimationSpeed"] = go->GetSpeed();
	savedStrings["AnimationClip"] = go->GetClipName();
	savedFloats["AnimationFrame"] = go->GetSampleTime();
}

void GraphicsObjectTexturedAnimated::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimated(ModelManager::GetModel(savedStrings["ModelName"]), TextureManager::GetTexture(savedStrings["TextureName"]));

	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	go->SetClip(savedStrings["AnimationClip"]);
	go->SetSampleTime(savedFloats["AnimationFrame"]);
	go->SetSpeed(savedFloats["AnimationSpeed"]);
	GraphicsObject3DComponent::Deserialize();
}
