#include "GOTexturedLitInstanced.h"

#include "../Model/Model.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../Shader/ShaderManager.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Renderer/Renderer.h"

#include <gl/glew.h>

std::list<unsigned int> GOTexturedLitInstanced::removedInstances;

GOTexturedLitInstanced::GOTexturedLitInstanced(Model* const model, Texture* diffuseTexture, Texture* specularTexture, unsigned int instanceCount) :
	GraphicsObject(model),
	GOLit(std::vector<Material>({ Material(diffuseTexture, specularTexture) })),
	translations(std::vector<glm::mat4>(instanceCount)),
	rotations(std::vector<glm::mat4>(instanceCount)),
	scales(std::vector<glm::mat4>(instanceCount)),
	transforms(std::vector<glm::mat4>(instanceCount)),
	rights(std::vector<glm::vec4>(instanceCount)),
	ups(std::vector<glm::vec4>(instanceCount)),
	forwards(std::vector<glm::vec4>(instanceCount)),
	trans(std::vector<glm::vec4>(instanceCount)),
	clipPlane(ShaderManager::GetClipPlane()),
	diffuse(diffuseTexture),
	spec(specularTexture)
{
	glCreateBuffers(1, &viewProjectionBuffer);
	glNamedBufferStorage(viewProjectionBuffer, sizeof(vp), &vp, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &lightSpaceMatrixBuffer);
	glNamedBufferStorage(lightSpaceMatrixBuffer, sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &clipPlaneBuffer);
	glNamedBufferStorage(clipPlaneBuffer, sizeof(clipPlane), &clipPlane, GL_DYNAMIC_STORAGE_BIT);

	glGenBuffers(1, &translationsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &rightBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &upBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &forwardBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * transforms.size(), &transforms[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(6, 1);

	glEnableVertexAttribArray(7);
	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(7, 1);

	glEnableVertexAttribArray(8);
	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(8, 1);

	FinalizeTransforms();

}

GOTexturedLitInstanced::~GOTexturedLitInstanced()
{
	glDeleteBuffers(1, &viewProjectionBuffer);
	glDeleteBuffers(1, &clipPlaneBuffer);
	glDeleteBuffers(1, &translationsBuffer);
	glDeleteBuffers(1, &rightBuffer);
	glDeleteBuffers(1, &upBuffer);
	glDeleteBuffers(1, &forwardBuffer);
	glDeleteBuffers(1, &lightSpaceMatrixBuffer);
}

void GOTexturedLitInstanced::Update()
{
	if (!Renderer::ShouldDraw())
	{
		return;
	}

	ShaderManager::StartShaderUsage("TexturedLitInstanced");


	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, ShaderManager::GetShadowMapTexture());

	float near_plane = -100.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);

	glm::vec3 position = CameraManager::GetActiveCamera().GetPosition();
	position.y = 10.0f;
	glm::vec3 zero = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<DirectionalLight*> directionalLights = LightManager::GetDirectionalLights(1);
	glm::vec3 lightDirection = (!directionalLights.empty()) ? directionalLights[0]->GetDirection() : glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 lightView = glm::lookAt(position,
		position + lightDirection,
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glBindBufferBase(GL_UNIFORM_BUFFER, 8, lightSpaceMatrixBuffer);
	glNamedBufferSubData(lightSpaceMatrixBuffer, 0, sizeof(glm::mat4), &lightSpaceMatrix);

	GOLit::UpdateLighting();

	model->BindBuffer();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, viewProjectionBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 10, clipPlaneBuffer);

	const Camera& cam = CameraManager::GetActiveCamera();
	vp.view = cam.GetView();
	vp.projection = cam.GetProjection();

	clipPlane.plane = ShaderManager::GetClipPlane();

	glNamedBufferSubData(viewProjectionBuffer, 0, sizeof(ViewProjectionUBO), &vp);
	glNamedBufferSubData(clipPlaneBuffer, 0, sizeof(ClipPlaneUBO), &clipPlane);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElementsInstanced(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0, (unsigned int)transforms.size());

	ShaderManager::EndShaderUsage("TexturedLitInstanced");
}

unsigned int GOTexturedLitInstanced::AddInstance()
{
	if (removedInstances.empty())
	{
		translations.push_back(glm::mat4(1.0f));
		rotations.push_back(glm::mat4(1.0f));
		scales.push_back(glm::mat4(1.0f));
		transforms.push_back(glm::mat4(1.0f));

		trans.push_back(glm::vec4(1.0f));
		rights.push_back(glm::vec4(1.0f));
		ups.push_back(glm::vec4(1.0f));
		forwards.push_back(glm::vec4(1.0f));

		return translations.size() - 1;
	}

	unsigned int newInstanceID = removedInstances.front();
	removedInstances.pop_front();

	translations[newInstanceID] = (glm::mat4(1.0f));
	rotations[newInstanceID] = (glm::mat4(1.0f));
	scales[newInstanceID] = (glm::mat4(1.0f));
	transforms[newInstanceID] = (glm::mat4(1.0f));

	trans[newInstanceID] = (glm::vec4(1.0f));
	rights[newInstanceID] = (glm::vec4(1.0f));
	ups[newInstanceID] = (glm::vec4(1.0f));
	forwards[newInstanceID] = (glm::vec4(1.0f));

	return newInstanceID;
}

void GOTexturedLitInstanced::RemoveInstanceByID(unsigned int instanceID)
{
	translations[instanceID] = glm::mat4(0.0f);
	rotations[instanceID] = glm::mat4(0.0f);
	scales[instanceID] = glm::mat4(0.0f);
	transforms[instanceID] = glm::mat4(0.0f);
	trans[instanceID] = glm::vec4(0.0f);
	rights[instanceID] = glm::vec4(0.0f);
	ups[instanceID] = glm::vec4(0.0f);
	forwards[instanceID] = glm::vec4(0.0f);

	removedInstances.push_back(instanceID);
}

unsigned int GOTexturedLitInstanced::GetInstanceCount()
{
	return static_cast<unsigned int>(transforms.size());
}

void GOTexturedLitInstanced::FinalizeTransforms()
{
	for (unsigned int i = 0; i < transforms.size(); ++i)
	{
		transforms[i] = translations[i] * rotations[i] * scales[i];
		rights[i] = transforms[i][0];
		ups[i] = transforms[i][1];
		forwards[i] = transforms[i][2];
		trans[i] = transforms[i][3];
	}

	glBindBuffer(GL_ARRAY_BUFFER, translationsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * trans.size(), trans.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, rightBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * rights.size(), rights.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, upBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * ups.size(), ups.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, forwardBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * forwards.size(), forwards.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GOTexturedLitInstanced::UpdateInstanceByID(unsigned int instanceID)
{
	transforms[instanceID] = translations[instanceID] * rotations[instanceID] * scales[instanceID];
	rights[instanceID] = transforms[instanceID][0];
	ups[instanceID] = transforms[instanceID][1];
	forwards[instanceID] = transforms[instanceID][2];
	trans[instanceID] = transforms[instanceID][3];

	glNamedBufferSubData(translationsBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &trans[instanceID]);
	glNamedBufferSubData(rightBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &rights[instanceID]);
	glNamedBufferSubData(upBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &ups[instanceID]);
	glNamedBufferSubData(forwardBuffer, instanceID * sizeof(glm::vec4), sizeof(glm::vec4), &forwards[instanceID]);
}

void GOTexturedLitInstanced::RenderToShadowMap()
{
	if (!Renderer::ShouldDraw())
	{
		return;
	}

	ShaderManager::StartShaderUsage("ShadowMapInstanced");

	float near_plane = -100.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);

	glm::vec3 position = CameraManager::GetActiveCamera().GetPosition();
	position.y = 10.0f;
	glm::vec3 zero = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<DirectionalLight*> directionalLights = LightManager::GetDirectionalLights(1);
	glm::vec3 lightDirection = (!directionalLights.empty()) ? directionalLights[0]->GetDirection() : glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 lightView = glm::lookAt(position,
		position + lightDirection,
		glm::vec3(0.0f, 1.0f, 0.0f));

	model->BindBuffer();

	vp.view = lightView;
	vp.projection = lightProjection;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, viewProjectionBuffer);
	glNamedBufferSubData(viewProjectionBuffer, 0, sizeof(vp), &vp);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElementsInstanced(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0, (unsigned int)transforms.size());

	ShaderManager::EndShaderUsage("ShadowMapInstanced");
}

Texture* GOTexturedLitInstanced::GetDiffuseTexture() const
{
	return diffuse;
}

Texture* GOTexturedLitInstanced::GetSpecularTexture() const
{
	return spec;
}

const std::vector<glm::mat4>& GOTexturedLitInstanced::GetInstanceTransforms() const
{
	return transforms;
}
