#include "GOTexturedLit.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"

GOTexturedLit::GOTexturedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap) :
	GOLit(diffuseMap, specularMap),
	GO3D(model)
{
	glCreateBuffers(1, &lightSpaceMatrixBuffer);
	glNamedBufferStorage(lightSpaceMatrixBuffer, sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

GOTexturedLit::~GOTexturedLit()
{
	glDeleteBuffers(1, &lightSpaceMatrixBuffer);
}

void GOTexturedLit::Update()
{
	ShaderManager::StartShaderUsage("TexturedLit");

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ShaderManager::GetShadowMapTexture());

	float near_plane = -100.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);

	glm::vec3 position = CameraManager::GetActiveCamera().GetPosition();
	position.y = 10.0f;
	glm::vec3 zero = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightDirection = LightManager::GetDirectionalLights(1)[0]->GetDirection();

	glm::mat4 lightView = glm::lookAt(position,
		position + lightDirection,
		glm::vec3(0.0f, 1.0f, 0.0f));

	mvp.view = lightView;
	mvp.projection = lightProjection;
	mvp.model = translation * rotation * scale;

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glBindBufferBase(GL_UNIFORM_BUFFER, 8, lightSpaceMatrixBuffer);
	glNamedBufferSubData(lightSpaceMatrixBuffer, 0, sizeof(glm::mat4), &lightSpaceMatrix);

	GOLit::UpdateLighting();
	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedLit");
}

void GOTexturedLit::RenderToShadowMap()
{
	ShaderManager::StartShaderUsage("ShadowMap");

	float near_plane = -100.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);

	glm::vec3 position = CameraManager::GetActiveCamera().GetPosition();
	position.y = 10.0f;
	glm::vec3 zero = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightDirection = LightManager::GetDirectionalLights(1)[0]->GetDirection();

	glm::mat4 lightView = glm::lookAt(position,
		position + lightDirection,
		glm::vec3(0.0f, 1.0f, 0.0f));

	model->BindBuffer();
	//const Camera& cam = CameraManager::GetCamera("ShadowMapDepth");
	mvp.view = lightView;
	mvp.projection = lightProjection;
	mvp.model = translation * rotation * scale;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpBuffer);
	glNamedBufferSubData(mvpBuffer, 0, sizeof(MVP), &mvp);
	
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)drawMode);
	glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);

	ShaderManager::EndShaderUsage("ShadowMap");
}
