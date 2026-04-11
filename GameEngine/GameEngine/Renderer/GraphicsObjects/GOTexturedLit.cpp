#include "GOTexturedLit.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"
#include "../Renderer/Renderer.h"
#include "GOTexturedLitInstanced.h"
#include "GraphicsObjectManager.h"
#include "../../Utils/Logger.h"


std::unordered_map<std::string, GOTexturedLitInstanced*> GOTexturedLit::instancedGraphicsObjects = std::unordered_map<std::string, GOTexturedLitInstanced*>();
std::unordered_map<std::string, std::list<GOTexturedLit*>> GOTexturedLit::instances = std::unordered_map<std::string, std::list<GOTexturedLit*>>();

GOTexturedLit::GOTexturedLit(Model* const model, const GOLit::Material& material) :
	GOLit(std::vector<Material>({ material })),
	GO3D(model, "TexturedLit"),
	instanceIdentifier(model->GetName() + "_" + material.diffuseMap->GetName() + "_" + material.specularMap->GetName() + "_" + material.normalMap->GetName())
{
	if (instancedGraphicsObjects.find(instanceIdentifier) == instancedGraphicsObjects.end())
	{
		instancedGraphicsObjects[instanceIdentifier] = nullptr;

		glCreateBuffers(1, &lightSpaceMatrixBuffer);
		glNamedBufferStorage(lightSpaceMatrixBuffer, sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
	}
	else
	{
		instanced = true;

		if (instancedGraphicsObjects[instanceIdentifier] == nullptr)
		{
			instancedGraphicsObjects[instanceIdentifier] = GraphicsObjectManager::CreateGO3DTexturedLitInstanced(model, material.diffuseMap, material.specularMap, material.normalMap, 1);
			instanceID = 0;

			GOTexturedLit* firstInstance = instances[instanceIdentifier].front();
			GOTexturedLitInstanced* instancedObj = instancedGraphicsObjects[instanceIdentifier];

			firstInstance->instanceID = instancedObj->AddInstance();
			instancedObj->SetTransform(firstInstance->GetTransform(), firstInstance->instanceID);
			instancedObj->SetRotation(firstInstance->GetRotation(), firstInstance->instanceID);
			instancedObj->SetScale(firstInstance->GetScale(), firstInstance->instanceID);
			instancedObj->SetShine(firstInstance->GetShine());
			instancedObj->SetTransform(firstInstance->GetTransform(), firstInstance->instanceID);
			firstInstance->instanced = true;

			instancedObj->UpdateInstanceByID(firstInstance->instanceID);

		}
		else
		{
			instanceID = instancedGraphicsObjects[instanceIdentifier]->AddInstance();
		}
	}

	instances[instanceIdentifier].push_back(this);
}

GOTexturedLit::~GOTexturedLit()
{
	instances[instanceIdentifier].remove(this);

	if (!instanced)
	{
		instancedGraphicsObjects.erase(instancedGraphicsObjects.find(instanceIdentifier));
		glDeleteBuffers(1, &lightSpaceMatrixBuffer);
	}
	else
	{
		if (!GraphicsObjectManager::Terminating())
		{
			instancedGraphicsObjects[instanceIdentifier]->RemoveInstanceByID(instanceID);
			instancedGraphicsObjects[instanceIdentifier]->FinalizeTransforms();

			if (instances[instanceIdentifier].size() == 1)
			{
				GraphicsObjectManager::Delete(instancedGraphicsObjects[instanceIdentifier]);
				instancedGraphicsObjects[instanceIdentifier] = nullptr;
				instances[instanceIdentifier].front()->instanced = false;
			}
		}
	}
}

void GOTexturedLit::Update()
{
	if (instanced)
		return;

	if (!Renderer::ShouldDraw())
	{
		return;
	}

	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, ShaderManager::GetShadowMapTexture());

	float near_plane = 0.10f, far_plane = 150.0f;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

	std::vector<DirectionalLight*> directionalLights = LightManager::GetDirectionalLights(1);
	glm::vec3 lightDirection = (!directionalLights.empty()) ? directionalLights[0]->GetDirection() : glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f) + -lightDirection * (far_plane / 2.0f);

	glm::mat4 lightView = glm::lookAt(position,
		position + lightDirection,
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glBindBufferBase(GL_UNIFORM_BUFFER, 8, lightSpaceMatrixBuffer);
	glNamedBufferSubData(lightSpaceMatrixBuffer, 0, sizeof(glm::mat4), &lightSpaceMatrix);

	GOLit::UpdateLighting();
	GO3D::Update();
}

void GOTexturedLit::RenderToShadowMap()
{
	if (instanced)
		return;

	if (!Renderer::ShouldDraw())
	{
		return;
	}

	ShaderManager::StartShaderUsage("ShadowMap");
	
	float near_plane = 0.10f, far_plane = 150.0f;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
	
	std::vector<DirectionalLight*> directionalLights = LightManager::GetDirectionalLights(1);
	glm::vec3 lightDirection = (!directionalLights.empty()) ? directionalLights[0]->GetDirection() : glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f) +  -lightDirection * (far_plane / 2.0f);
	
	glm::mat4 lightView = glm::lookAt(position,
		position + lightDirection,
		glm::vec3(0.0f, 1.0f, 0.0f));
	
	model->BindBuffer();
	mvp.view = lightView;
	mvp.projection = lightProjection;
	mvp.model = translation * rotation * scale;
	
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpBuffer);
	glNamedBufferSubData(mvpBuffer, 0, sizeof(MVP), &mvp);
	
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)drawMode);
	glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	
	ShaderManager::EndShaderUsage("ShadowMap");
}

void GOTexturedLit::Translate(const glm::vec3& t)
{
	GO3D::Translate(t);

	if (instanced)
	{
		instancedGraphicsObjects[instanceIdentifier]->Translate(t, instanceID);
		instancedGraphicsObjects[instanceIdentifier]->UpdateInstanceByID(instanceID);
	}
};

void GOTexturedLit::Rotate(float angle, const glm::vec3& axis)
{
	GO3D::Rotate(angle, axis);

	if (instanced)
	{
		instancedGraphicsObjects[instanceIdentifier]->Rotate(angle, axis, instanceID);
		instancedGraphicsObjects[instanceIdentifier]->UpdateInstanceByID(instanceID);
	}
};

void GOTexturedLit::Scale(const glm::vec3& s)
{
	GO3D::Scale(s);

	if (instanced)
	{
		instancedGraphicsObjects[instanceIdentifier]->Scale(s, instanceID);
		instancedGraphicsObjects[instanceIdentifier]->UpdateInstanceByID(instanceID);
	}
};

void GOTexturedLit::SetTranslation(const glm::vec3& t)
{
	GO3D::SetTranslation(t);

	if (instanced)
	{
		instancedGraphicsObjects[instanceIdentifier]->SetTranslation(t, instanceID);
		instancedGraphicsObjects[instanceIdentifier]->UpdateInstanceByID(instanceID);
	}
};

void GOTexturedLit::SetRotation(const glm::mat4& newRotation)
{
	GO3D::SetRotation(newRotation);

	if (instanced)
	{
		instancedGraphicsObjects[instanceIdentifier]->SetRotation(newRotation, instanceID);
		instancedGraphicsObjects[instanceIdentifier]->UpdateInstanceByID(instanceID);
	}
};

void GOTexturedLit::SetScale(const glm::vec3& s)
{
	GO3D::SetScale(s);

	if (instanced)
	{
		instancedGraphicsObjects[instanceIdentifier]->SetScale(s, instanceID);
		instancedGraphicsObjects[instanceIdentifier]->UpdateInstanceByID(instanceID);
	}
};

glm::vec3 GOTexturedLit::GetTranslation() const
{
	if (instanced)
	{
		return instancedGraphicsObjects[instanceIdentifier]->GetTranslation(instanceID);
	}

	return GO3D::GetTranslation();
};

glm::vec3 GOTexturedLit::GetScale() const
{
	if (instanced)
	{
		return instancedGraphicsObjects[instanceIdentifier]->GetScale(instanceID);
	}

	return GO3D::GetScale();
};

glm::mat4 GOTexturedLit::GetRotation() const
{
	if (instanced)
	{
		return instancedGraphicsObjects[instanceIdentifier]->GetRotation(instanceID);
	}

	return GO3D::GetRotation();
};

void GOTexturedLit::SetTransform(const glm::mat4& newTransform)
{
	GO3D::SetTransform(newTransform);

	if (instanced)
	{
		instancedGraphicsObjects[instanceIdentifier]->SetTransform(newTransform, instanceID);
		instancedGraphicsObjects[instanceIdentifier]->UpdateInstanceByID(instanceID);
	}
};

const glm::mat4& GOTexturedLit::GetTransform()
{
	return transformation = translation * rotation * scale;
};

Texture* const GOTexturedLit::GetDiffuseTexture() const
{
	return materials.front().diffuseMap;
}

Texture* const GOTexturedLit::GetSpecularTexture() const
{
	return materials.front().specularMap;
}

Texture* const GOTexturedLit::GetNormalTexture() const
{
	return materials.front().normalMap;
}