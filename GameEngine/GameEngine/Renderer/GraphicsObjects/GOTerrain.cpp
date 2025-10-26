#include "GOTerrain.h"
#include "../Shader/ShaderManager.h"
#include "../Camera/CameraManager.h"
#include "../Light/LightManager.h"
#include "../Light/DirectionalLight.h"
#include "../Texture/TextureManager.h"
#include "../Texture/Texture.h"
#include "../Model/Model.h"

GOTerrain::GOTerrain(Model* const model, const std::vector<Material>& textures, const std::string& blendMap) :
	GOLit(textures),
	GO3D(model),
	blendMap(TextureManager::GetTexture(blendMap))
{
	glCreateBuffers(1, &lightSpaceMatrixBuffer);
	glNamedBufferStorage(lightSpaceMatrixBuffer, sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);

	
}

GOTerrain::~GOTerrain()
{
	glDeleteBuffers(1, &lightSpaceMatrixBuffer);
}

void GOTerrain::Update()
{
	ShaderManager::StartShaderUsage("Terrain");

	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, ShaderManager::GetShadowMapTexture());

	if (blendMap != nullptr)
	{
		blendMap->Bind(GL_TEXTURE30);
	}

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
	GO3D::Update();

	ShaderManager::EndShaderUsage("Terrain");
}

void GOTerrain::RenderToShadowMap()
{
	ShaderManager::StartShaderUsage("ShadowMap");

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

	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)drawMode);
	glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);

	ShaderManager::EndShaderUsage("ShadowMap");
}