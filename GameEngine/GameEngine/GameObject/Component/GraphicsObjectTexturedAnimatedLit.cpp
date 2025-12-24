#include "GraphicsObjectTexturedAnimatedLit.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTexturedAnimatedLit.h"
#include "../../Renderer/Model/Model.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Texture/Texture.h"
#include "../../Renderer/Texture/TextureManager.h"
#include "../../Renderer/Animation/Armature.h"

GraphicsObjectTexturedAnimatedLit::GraphicsObjectTexturedAnimatedLit()
{
	RegisterComponentClassType<GraphicsObjectTexturedAnimatedLit>(this);
}

GraphicsObjectTexturedAnimatedLit::GraphicsObjectTexturedAnimatedLit(Model* const model, Texture* const diffuse, Texture* const specular)
{
	RegisterComponentClassType<GraphicsObjectTexturedAnimatedLit>(this);
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(model, diffuse, specular);
}

GraphicsObjectTexturedAnimatedLit::~GraphicsObjectTexturedAnimatedLit()
{
	GraphicsObjectManager::Delete(graphics);
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

glm::mat4 GraphicsObjectTexturedAnimatedLit::GetJointTransform(const std::string& jointName) const
{
	GOTexturedAnimatedLit* g = static_cast<GOTexturedAnimatedLit*>(graphics);

	const std::vector<std::string>& jointNames = g->GetModel()->GetArmature()->GetJointNames();

	unsigned int i = 0;
	for (const auto& name : jointNames)
	{
		if (name == jointName)
		{
			break;
		}
		i++;
	}

	return g->GetTransform() * g->GetAnimPoseArray()[i] * g->GetAnimInvBindPoseArray()[i];
}

void GraphicsObjectTexturedAnimatedLit::SetRenderGraphics(bool val)
{
	graphics->SetRenderGraphics(val);
}

void GraphicsObjectTexturedAnimatedLit::SetRenderShadow(bool val)
{
	graphics->SetRenderShadow(val);
}

void GraphicsObjectTexturedAnimatedLit::SetRenderReflection(bool val)
{
	graphics->SetRenderReflection(val);
}

bool GraphicsObjectTexturedAnimatedLit::RenderGraphics() const
{
	return graphics->RenderGraphics();
}

bool GraphicsObjectTexturedAnimatedLit::RenderShadow() const
{
	return graphics->RenderShadow();
}

bool GraphicsObjectTexturedAnimatedLit::RenderReflection() const
{
	return graphics->RenderReflection();
}

void GraphicsObjectTexturedAnimatedLit::Serialize()
{
	GraphicsObject3DComponent::Serialize();

	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	savedStrings["ModelName"] = go->GetModel()->GetName();
	savedStrings["DiffuseTextureName"] = go->GetDiffuseTexture()->GetName();
	savedStrings["SpecularTextureName"] = go->GetSpecularTexture()->GetName();
	savedFloats["AnimationSpeed"] = go->GetSpeed();
	savedInts["AnimationClip"] = go->GetClip();
	savedInts["AnimationFrame"] = go->GetFrame();
	savedFloats["Shine"] = go->GetShine();
}

void GraphicsObjectTexturedAnimatedLit::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel(savedStrings["ModelName"]), TextureManager::GetTexture(savedStrings["DiffuseTextureName"]), TextureManager::GetTexture(savedStrings["SpecularTextureName"]));

	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	go->SetClip(savedInts["AnimationClip"]);
	go->SetFrame(savedInts["AnimationFrame"]);
	go->SetSpeed(savedFloats["AnimationSpeed"]);
	go->SetShine(savedFloats["Shine"]);
	GraphicsObject3DComponent::Deserialize();
}
