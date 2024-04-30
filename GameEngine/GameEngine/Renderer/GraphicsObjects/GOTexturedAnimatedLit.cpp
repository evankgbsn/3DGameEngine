#include "GOTexturedAnimatedLit.h"

#include "../Shader/ShaderManager.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Light/PointLight.h"
#include "../Texture/Texture.h"
#include "../Model/Model.h"
#include "../Animation/Animation.h"
#include "../Animation/Armature.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"

#include <GL/glew.h>

GOTexturedAnimatedLit::GOTexturedAnimatedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap) :
	GOLit(diffuseMap, specularMap),
	GO3DAnimated(model)
{
	glCreateBuffers(1, &lightSpaceMatrixBuffer);
	glNamedBufferStorage(lightSpaceMatrixBuffer, sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

GOTexturedAnimatedLit::~GOTexturedAnimatedLit()
{
	glDeleteBuffers(1, &lightSpaceMatrixBuffer);
}

void GOTexturedAnimatedLit::Update()
{
	ShaderManager::StartShaderUsage("TexturedAnimatedLit");


	glActiveTexture(GL_TEXTURE2);
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

	mvp.view = lightView;
	mvp.projection = lightProjection;
	mvp.model = translation * rotation * scale;

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glBindBufferBase(GL_UNIFORM_BUFFER, 8, lightSpaceMatrixBuffer);
	glNamedBufferSubData(lightSpaceMatrixBuffer, 0, sizeof(glm::mat4), &lightSpaceMatrix);

	GOLit::UpdateLighting();
	GO3DAnimated::Update();
	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedAnimatedLit");
}

void GOTexturedAnimatedLit::RenderToShadowMap()
{
	ShaderManager::StartShaderUsage("AnimatedShadowMap");

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
	//const Camera& cam = CameraManager::GetCamera("ShadowMapDepth");
	mvp.view = lightView;
	mvp.projection = lightProjection;
	mvp.model = translation * rotation * scale;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mvpBuffer);
	glNamedBufferSubData(mvpBuffer, 0, sizeof(MVP), &mvp);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, animationBuffer);
	glNamedBufferSubData(animationBuffer, 0, sizeof(AnimationData), &animationData);

	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)drawMode);
	glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);

	ShaderManager::EndShaderUsage("AnimatedShadowMap");
}
