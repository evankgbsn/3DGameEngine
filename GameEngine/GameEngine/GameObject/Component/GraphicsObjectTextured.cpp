#include "GraphicsObjectTextured.h"

#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/GOTextured.h"
#include "../../Renderer/Texture/TextureManager.h"
#include "../../Renderer/Texture/Texture.h"
#include "../../Renderer/Model/Model.h"
#include "../../Renderer/Model/ModelManager.h"

GraphicsObjectTextured::GraphicsObjectTextured(Model* const model, Texture* const texture)
{
	RegisterComponentClassType<GraphicsObjectTextured>(this);
	graphics = GraphicsObjectManager::Create3DGOTextured(model, texture);
}

GraphicsObjectTextured::~GraphicsObjectTextured()
{
}

void GraphicsObjectTextured::Serialize() 
{
	GraphicsObject3DComponent::Serialize();

	GOTextured* go = static_cast<GOTextured*>(graphics);

	savedStrings["TextureName"] = go->GetTexture()->GetName();
	savedStrings["ModelName"] = go->GetModel()->GetName();
}

void GraphicsObjectTextured::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::Create3DGOTextured(ModelManager::GetModel(savedStrings["ModelName"]), TextureManager::GetTexture(savedStrings["TextureName"]));

	GraphicsObject3DComponent::Deserialize();
}


