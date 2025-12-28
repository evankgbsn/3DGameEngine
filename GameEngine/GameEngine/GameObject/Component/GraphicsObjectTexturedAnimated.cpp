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

void GraphicsObjectTexturedAnimated::Serialize()
{
	GraphicsObject3DComponent::Serialize();

	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	savedStrings["ModelName"] = GetModel()->GetName();
	savedStrings["TextureName"] = go->GetTexture()->GetName();
	savedFloats["AnimationSpeed"] = go->GetSpeed();
	savedInts["AnimationClip"] = go->GetClip();
	savedInts["AnimationFrame"] = go->GetFrame();
}

void GraphicsObjectTexturedAnimated::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimated(ModelManager::GetModel(savedStrings["ModelName"]), TextureManager::GetTexture(savedStrings["TextureName"]));

	GOTexturedAnimated* go = static_cast<GOTexturedAnimated*>(graphics);

	go->SetClip(savedInts["AnimationClip"]);
	go->SetFrame(savedInts["AnimationFrame"]);
	go->SetSpeed(savedFloats["AnimationSpeed"]);
	GraphicsObject3DComponent::Deserialize();
}
