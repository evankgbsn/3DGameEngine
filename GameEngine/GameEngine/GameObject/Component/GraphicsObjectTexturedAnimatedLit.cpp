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

GraphicsObjectTexturedAnimatedLit::GraphicsObjectTexturedAnimatedLit(Model* const model, const std::string& diffuse, const std::string& specular, const std::string& normal)
{
	RegisterComponentClassType<GraphicsObjectTexturedAnimatedLit>(this);
	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(model, TextureManager::GetTexture(diffuse), TextureManager::GetTexture(specular), TextureManager::GetTexture(normal));
}

GraphicsObjectTexturedAnimatedLit::~GraphicsObjectTexturedAnimatedLit()
{
	GraphicsObjectManager::Delete(graphics);
}

void GraphicsObjectTexturedAnimatedLit::SetShine(float shine)
{
	static_cast<GOTexturedAnimatedLit*>(graphics)->SetShine(shine);
}

void GraphicsObjectTexturedAnimatedLit::SetClip(const std::string& clipName)
{
	static_cast<GOTexturedAnimatedLit*>(graphics)->SetClip(clipName);
}

void GraphicsObjectTexturedAnimatedLit::SetSpeed(float speed)
{
	static_cast<GOTexturedAnimatedLit*>(graphics)->SetSpeed(speed);
}

glm::mat4 GraphicsObjectTexturedAnimatedLit::GetJointTransform(const std::string& jointName) const
{
	GOTexturedAnimatedLit* g = static_cast<GOTexturedAnimatedLit*>(graphics);

	const std::vector<std::string>& jointNames = g->GetModel()->GetArmature()->GetJointNames();

	for (unsigned int i = 0; i < jointNames.size(); i++)
	{
		if (jointName == jointNames[i])
		{
			return g->GetTransform() * g->GetAnimPoseArray()[i] * glm::inverse(g->GetAnimInvBindPoseArray()[i]);
		}
	}

	return glm::mat4(1.0f);
}

const std::string& GraphicsObjectTexturedAnimatedLit::GetCurrentAnimation() const
{
	GOTexturedAnimatedLit * go = static_cast<GOTexturedAnimatedLit*>(graphics);
	return go->GetClipName();
}

void GraphicsObjectTexturedAnimatedLit::FadeAnimationTo(const std::string& animation, float time)
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);
	go->FadeTo(animation, time);
}

void GraphicsObjectTexturedAnimatedLit::InitializeAdditiveAnimation(const std::string& animation)
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);
	go->InitializeAdditiveAnimation(animation);
}

void GraphicsObjectTexturedAnimatedLit::SetAdditiveAnimationTime(const std::string& animation, float time)
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);
	go->SetAdditiveAnimationTime(animation, time);
}

float GraphicsObjectTexturedAnimatedLit::GetAdditiveAnimationTime(const std::string& animation) const
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);
	return go->GetAdditiveAnimationTime(animation);
}

bool GraphicsObjectTexturedAnimatedLit::isFading() const
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	return go->isFading();
}

float GraphicsObjectTexturedAnimatedLit::GetFadeToTime() const
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	return go->GetFadeToTime();
}

const std::string& GraphicsObjectTexturedAnimatedLit::GetFadeToClipName() const
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	return go->GetFadeToClipName();
}

void GraphicsObjectTexturedAnimatedLit::RegisterAnimationStartCallback(const std::string& name, std::function<void(const std::string&)>* callback)
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	go->RegisterAnimationStartCallback(name, callback);
}

void GraphicsObjectTexturedAnimatedLit::DeregisterAnimationStartCallback(const std::string& name)
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	go->DeregisterAnimationStartCallback(name);
}

void GraphicsObjectTexturedAnimatedLit::RegisterAnimationStopCallback(const std::string& name, std::function<void(const std::string&)>* callback)
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	go->RegisterAnimationStopCallback(name, callback);
}

void GraphicsObjectTexturedAnimatedLit::DeregisterAnimationStopCallback(const std::string& name)
{
	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	go->DeregisterAnimationStopCallback(name);
}

void GraphicsObjectTexturedAnimatedLit::Serialize()
{
	GraphicsObject3DComponent::Serialize();

	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	savedStrings["ModelName"] = go->GetModel()->GetName();
	savedStrings["DiffuseTextureName"] = go->GetDiffuseTexture()->GetName();
	savedStrings["SpecularTextureName"] = go->GetSpecularTexture()->GetName();
	savedStrings["NormalTextureName"] = go->GetNormalTexture()->GetName();
	savedFloats["AnimationSpeed"] = go->GetSpeed();
	savedStrings["AnimationClip"] = go->GetClipName();
	savedFloats["AnimationFrame"] = go->GetSampleTime();
	savedFloats["Shine"] = go->GetShine();
}

void GraphicsObjectTexturedAnimatedLit::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DTexturedAnimatedLit(ModelManager::GetModel(savedStrings["ModelName"]), TextureManager::GetTexture(savedStrings["DiffuseTextureName"]), TextureManager::GetTexture(savedStrings["SpecularTextureName"]), TextureManager::GetTexture(savedStrings["NormalTextureName"]));

	GOTexturedAnimatedLit* go = static_cast<GOTexturedAnimatedLit*>(graphics);

	go->SetClip(savedStrings["AnimationClip"]);
	go->SetSampleTime(savedFloats["AnimationFrame"]);
	go->SetSpeed(savedFloats["AnimationSpeed"]);
	go->SetShine(savedFloats["Shine"]);
	GraphicsObject3DComponent::Deserialize();
}
