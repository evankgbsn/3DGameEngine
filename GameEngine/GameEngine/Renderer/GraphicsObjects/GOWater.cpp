#include "GOWater.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/TextureManager.h"
#include "../Texture/Texture.h"
#include "../Time/TimeManager.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"

GOWater::GOWater(Model* const model) :
	GO3D(model),
	effects(
		{
			glm::vec4(0.0f, 0.5f, 0.8f, 1.0f),
			0.6f,
			0.0f,
			0.003f,
			.5f
		}
	),
	fog({ 0.007f, 2.5f })
{
	glCreateBuffers(1, &effectsBuffer);
	glNamedBufferStorage(effectsBuffer, sizeof(Effects), &effects, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &camPositionBuffer);
	glNamedBufferStorage(camPositionBuffer, sizeof(glm::vec4), &camPosition, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &fogBuffer);
	glNamedBufferStorage(fogBuffer, sizeof(FogEffect), &fog, GL_DYNAMIC_STORAGE_BIT);
}

GOWater::~GOWater()
{
	glDeleteBuffers(1, &effectsBuffer);
	glDeleteBuffers(1, &camPositionBuffer);
	glDeleteBuffers(1, &fogBuffer);
}

void GOWater::Update()
{
	ShaderManager::StartShaderUsage("Water");

	TextureManager::GetTexture("ReflectionTexture")->Bind(GL_TEXTURE0);
	TextureManager::GetTexture("RefractionTexture")->Bind(GL_TEXTURE1);
	TextureManager::GetTexture("DuDv")->Bind(GL_TEXTURE2);

	effects.moveFactor += 0.007f * TimeManager::DeltaTime();
	
	if (effects.moveFactor >= 1)
	{
		effects.moveFactor = effects.moveFactor - 1.0f;
	}

	camPosition = glm::vec4(CameraManager::GetActiveCamera().GetPosition(), 1.0f);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, effectsBuffer);
	glNamedBufferSubData(effectsBuffer, 0, sizeof(Effects), &effects);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, camPositionBuffer);
	glNamedBufferSubData(camPositionBuffer, 0, sizeof(glm::vec4), &camPosition);

	glBindBufferBase(GL_UNIFORM_BUFFER, 11, fogBuffer);
	glNamedBufferSubData(fogBuffer, 0, sizeof(FogEffect), &fog);

	GO3D::Update();

	ShaderManager::EndShaderUsage("Water");
}
