#include "GraphicsObjectTexturedLitInstanced.h"

#include "../../Renderer/GraphicsObjects/GOTexturedLitInstanced.h"
#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/Texture/TextureManager.h"
#include "../../Renderer/Texture/Texture.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Model/Model.h"

GraphicsObjectTexturedLitInstanced::GraphicsObjectTexturedLitInstanced() :
	graphics(nullptr)
{
	RegisterComponentClassType<GraphicsObjectTexturedLitInstanced>(this);
}

GraphicsObjectTexturedLitInstanced::GraphicsObjectTexturedLitInstanced(const std::string& modelName, const std::string& diffuseTextureName, const std::string& specularTextureName, unsigned int instanceCount) :
	Component(),
	graphics(nullptr)
{
	RegisterComponentClassType<GraphicsObjectTexturedLitInstanced>(this);

	Texture* diffuse = TextureManager::GetTexture(diffuseTextureName);
	Texture* specular = TextureManager::GetTexture(specularTextureName);
	Model* model = ModelManager::GetModel(modelName);

	graphics = GraphicsObjectManager::CreateGO3DTexturedLitInstanced(model, diffuse, specular, instanceCount);
}

GraphicsObjectTexturedLitInstanced::~GraphicsObjectTexturedLitInstanced()
{
	GraphicsObjectManager::Delete(graphics);
}

void GraphicsObjectTexturedLitInstanced::Translate(const glm::vec3& t, unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->Translate(t, instanceID);
	}
}

void GraphicsObjectTexturedLitInstanced::Rotate(float angle, const glm::vec3& axis, unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->Rotate(angle, axis, instanceID);
	}
};

void GraphicsObjectTexturedLitInstanced::Scale(const glm::vec3& s, unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->Scale(s, instanceID);
	}
};

void GraphicsObjectTexturedLitInstanced::SetTranslation(const glm::vec3& t, unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->SetTranslation(t, instanceID);
	}
};

void GraphicsObjectTexturedLitInstanced::SetRotation(const glm::mat4& newRotation, unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->SetRotation(newRotation, instanceID);
	}
};

void GraphicsObjectTexturedLitInstanced::SetScale(const glm::vec3& s, unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->SetScale(s, instanceID);
	}
};

glm::vec3 GraphicsObjectTexturedLitInstanced::GetTranslation(unsigned int instanceID) const
{
	if (graphics != nullptr)
	{
		return graphics->GetTranslation(instanceID);
	}

	return glm::vec3(0.0f);
};

glm::vec3 GraphicsObjectTexturedLitInstanced::GetScale(unsigned int instanceID) const
{
	if (graphics != nullptr)
	{
		return graphics->GetScale(instanceID);
	}

	return glm::vec3(0.0f);
};

glm::mat4 GraphicsObjectTexturedLitInstanced::GetRotation(unsigned int instanceID) const
{
	if (graphics != nullptr)
	{
		return graphics->GetRotation(instanceID);
	}

	return glm::mat4(1.0f);
};

void GraphicsObjectTexturedLitInstanced::SetTransform(const glm::mat4& newTransform, unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->SetTransform(newTransform, instanceID);
	}
};

glm::mat4 GraphicsObjectTexturedLitInstanced::GetTransform(unsigned int instanceID) const
{
	if (graphics != nullptr)
	{
		return graphics->GetTransform(instanceID);
	}

	return glm::mat4(1.0f);
};

void GraphicsObjectTexturedLitInstanced::FinalizeTransforms()
{
	if (graphics != nullptr)
	{
		graphics->FinalizeTransforms();
	}
}

void GraphicsObjectTexturedLitInstanced::UpdateInstanceByID(unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		graphics->UpdateInstanceByID(instanceID);
	}
}

void GraphicsObjectTexturedLitInstanced::Update()
{
}

void GraphicsObjectTexturedLitInstanced::Serialize()
{
	if (graphics != nullptr)
	{
		savedStrings["ModelName"] = graphics->GetModel()->GetName();
		savedStrings["DiffuseTextureName"] = graphics->GetDiffuseTexture()->GetName();
		savedStrings["SpecularTextureName"] = graphics->GetSpecularTexture()->GetName();
		savedFloats["Shine"] = graphics->GetShine();
		savedInts["InstanceCount"] = graphics->GetInstanceCount();

		unsigned int i = 0;
		for (const auto& transform : graphics->GetInstanceTransforms())
		{
			savedMat4s[std::to_string(i)] = transform;
		}
	}
}

void GraphicsObjectTexturedLitInstanced::Deserialize()
{
	if (graphics != nullptr)
	{
		GraphicsObjectManager::Delete(graphics);
	}

	graphics = GraphicsObjectManager::CreateGO3DTexturedLitInstanced(ModelManager::GetModel(savedStrings["ModelName"]), TextureManager::GetTexture(savedStrings["DiffuseTextureName"]), TextureManager::GetTexture(savedStrings["SpecularTextureName"]), savedInts["InstanceCount"]);

	for (unsigned int i = 0; i < graphics->GetInstanceCount(); i++)
	{
		graphics->SetTransform(savedMat4s[std::to_string(i)], i);
	}

	SetShine(savedFloats["Shine"]);
}

void GraphicsObjectTexturedLitInstanced::SetShine(float shine)
{
	if (graphics != nullptr)
	{
		graphics->SetShine(shine);
	}
}

unsigned int GraphicsObjectTexturedLitInstanced::AddInstance()
{
	if (graphics != nullptr)
	{
		return graphics->AddInstance();
	}

	return UINT32_MAX;
}

void GraphicsObjectTexturedLitInstanced::RemoveInstanceByID(unsigned int instanceID)
{
	if (graphics != nullptr)
	{
		return graphics->RemoveInstanceByID(instanceID);
	}
}

GOTexturedLitInstanced* GraphicsObjectTexturedLitInstanced::GetGraphics() const
{
	return graphics;
}