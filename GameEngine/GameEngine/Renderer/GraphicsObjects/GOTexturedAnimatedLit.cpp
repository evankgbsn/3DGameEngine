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

GOTexturedAnimatedLit::GOTexturedAnimatedLit(Model* const model, Texture* const diffuseMap, Texture* const specularMap, Texture* const normalMap) :
	GOLit(std::vector<Material>({ Material(diffuseMap, specularMap, normalMap) })),
	GO3DAnimated(model, "TexturedAnimatedLit")
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

	mvp.view = lightView;
	mvp.projection = lightProjection;
	//mvp.model = translation * rotation * scale;

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	glBindBufferBase(GL_UNIFORM_BUFFER, 8, lightSpaceMatrixBuffer);
	glNamedBufferSubData(lightSpaceMatrixBuffer, 0, sizeof(glm::mat4), &lightSpaceMatrix);

	GOLit::UpdateLighting();
	GO3DAnimated::Update();
	GO3D::Update();
}

void GOTexturedAnimatedLit::RenderToShadowMap()
{
	ShaderManager::StartShaderUsage("AnimatedShadowMap");

	float near_plane = 0.10f, far_plane = 150.0f;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

	std::vector<DirectionalLight*> directionalLights = LightManager::GetDirectionalLights(1);
	glm::vec3 lightDirection = (!directionalLights.empty()) ? directionalLights[0]->GetDirection() : glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f) + -lightDirection * (far_plane / 2.0f);

	glm::mat4 lightView = glm::lookAt(position,
		position + lightDirection,
		glm::vec3(0.0f, 1.0f, 0.0f));

	model->BindBuffer();
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

Texture* const GOTexturedAnimatedLit::GetDiffuseTexture() const
{
	return materials.front().diffuseMap;
}

Texture* const GOTexturedAnimatedLit::GetSpecularTexture() const
{
	return materials.front().specularMap;
}

Texture* const GOTexturedAnimatedLit::GetNormalTexture() const
{
	return materials.front().normalMap;
}

