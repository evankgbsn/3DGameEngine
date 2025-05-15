#include "GraphicsObjectTexturedLit.h"

#include "../../Renderer/GraphicsObjects/GOTexturedLit.h"
#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/Model/Model.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Texture/Texture.h"
#include "../../Renderer/Texture/TextureManager.h"

GraphicsObjectTexturedLit::GraphicsObjectTexturedLit(Model* const model, Texture* const diffuse, Texture* const specular)
{
	RegisterComponentClassType<GraphicsObjectTexturedLit>(this);
	graphics = GraphicsObjectManager::CreateGO3DTexturedLit(model, diffuse, specular);
}

GraphicsObjectTexturedLit::~GraphicsObjectTexturedLit()
{
}

void GraphicsObjectTexturedLit::SetShine(float shine)
{
	static_cast<GOTexturedLit*>(graphics)->SetShine(shine);
}

void GraphicsObjectTexturedLit::Serialize()
{
	GraphicsObject3DComponent::Serialize();

	GOTexturedLit* go = static_cast<GOTexturedLit*>(graphics);

	savedStrings["ModelName"] = GetModel()->GetName();
	savedStrings["DiffuseTextureName"] = go->GetDiffuseTexture()->GetName();
	savedStrings["SpecularTextureName"] = go->GetSpecularTexture()->GetName();
	savedFloats["Shine"] = go->GetShine();
}

void GraphicsObjectTexturedLit::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel(savedStrings["ModelName"]), TextureManager::GetTexture(savedStrings["DiffuseTextureName"]), TextureManager::GetTexture(savedStrings["SpecularTextureName"]));

	SetShine(savedFloats["Shine"]);

	GraphicsObject3DComponent::Deserialize();
}
