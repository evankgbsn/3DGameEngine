#include "GraphicsObjectTexturedLit.h"

#include "../../Renderer/GraphicsObjects/GOTexturedLit.h"
#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/Model/Model.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Texture/Texture.h"
#include "../../Renderer/Texture/TextureManager.h"

GraphicsObjectTexturedLit::GraphicsObjectTexturedLit(Model* const model, const std::string& diffuse, const std::string& specular, const std::string& normal)
{
	RegisterComponentClassType<GraphicsObjectTexturedLit>(this);
	graphics = GraphicsObjectManager::CreateGO3DTexturedLit(model, GOLit::Material(TextureManager::GetTexture(diffuse), TextureManager::GetTexture(specular), TextureManager::GetTexture(normal)));
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
	savedStrings["NormalTextureName"] = go->GetNormalTexture()->GetName();
	savedFloats["Shine"] = go->GetShine();
}

void GraphicsObjectTexturedLit::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DTexturedLit(ModelManager::GetModel(savedStrings["ModelName"]), GOLit::Material(TextureManager::GetTexture(savedStrings["DiffuseTextureName"]), TextureManager::GetTexture(savedStrings["SpecularTextureName"]), TextureManager::GetTexture(savedStrings["NormalTextureName"])));

	SetShine(savedFloats["Shine"]);

	GraphicsObject3DComponent::Deserialize();
}
