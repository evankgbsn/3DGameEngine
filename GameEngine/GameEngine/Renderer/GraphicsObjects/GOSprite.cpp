#include "GOSprite.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"
#include "../Window/WindowManager.h"
#include "../Model/Model.h"
#include "../../Math/Math.h"
#include "../../Time/TimeManager.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

GOSprite::GOSprite(Model* model, Texture* texture, const glm::vec2& initialPosition) :
	GraphicsObject(model),
	modelMat(1.0f),
	imageTexture(texture)
{
	glCreateBuffers(1, &projectionBuffer);
	glNamedBufferStorage(projectionBuffer, sizeof(projection), &projection, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, sizeof(modelMat), &modelMat, GL_DYNAMIC_STORAGE_BIT);

	SetPosition(initialPosition);
}

GOSprite::~GOSprite()
{
	glDeleteBuffers(1, &projectionBuffer);
	glDeleteBuffers(1, &positionBuffer);
}

glm::vec2 GOSprite::GetPosition() const
{
	return glm::vec2(modelMat[3]);
}

void GOSprite::SetPosition(const glm::vec2& newPosition)
{
	modelMat[3] = glm::vec4(newPosition, modelMat[3][2], modelMat[3][3]);
}

void GOSprite::Update()
{
	ShaderManager::StartShaderUsage("Sprite");

	model->BindBuffer();

	imageTexture->Bind(GL_TEXTURE0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, projectionBuffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, positionBuffer);

	Window* const window = WindowManager::GetWindow("Engine");
	float windowWidth = (float)window->GetWidth();
	float windowHeight = (float)window->GetHeight();

	projection = glm::ortho(0.0f, windowWidth, 0.0f, windowHeight);

	glNamedBufferSubData(projectionBuffer, 0, sizeof(projection), &projection);
	glNamedBufferSubData(positionBuffer, 0, sizeof(modelMat), &modelMat);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);

	ShaderManager::EndShaderUsage("Sprite");
}

void GOSprite::Translate(const glm::vec2& translation)
{
	modelMat[3] += glm::vec4(translation, 0.0f, 0.0f);
}

void GOSprite::SetTexture(Texture* const newTexture)
{
	imageTexture = newTexture;
}

void GOSprite::SetScale(float x, float y)
{
	glm::vec3 currentScale(glm::length(glm::vec3(modelMat[0])), glm::length(glm::vec3(modelMat[1])), glm::length(glm::vec3(modelMat[2])));

	modelMat = { glm::normalize(modelMat[0]), glm::normalize(modelMat[1]) , glm::normalize(modelMat[2]) , modelMat[3] };

	modelMat = glm::scale(modelMat, glm::vec3(x, y, 1.0f));
}

void GOSprite::SetZ(float z)
{
	modelMat[3][2] = z;
}

glm::vec2 GOSprite::GetScale() const
{
	glm::vec3 x = modelMat[0];
	glm::vec3 y = modelMat[1];

	return { glm::length(x), glm::length(y) };
}

glm::mat4 GOSprite::GetProjection() const
{
	return projection;
}

glm::mat4 GOSprite::GetModelMat() const
{
	return modelMat;
}

Texture* const GOSprite::GetTexture() const
{
	return imageTexture;
}
