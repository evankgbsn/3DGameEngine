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
	position(initialPosition),
	imageTexture(texture)
{
	glCreateBuffers(1, &projectionBuffer);
	glNamedBufferStorage(projectionBuffer, sizeof(projection), &projection, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &positionBuffer);
	glNamedBufferStorage(positionBuffer, sizeof(position), &position, GL_DYNAMIC_STORAGE_BIT);
}

GOSprite::~GOSprite()
{
	glDeleteBuffers(1, &projectionBuffer);
	glDeleteBuffers(1, &positionBuffer);
}

glm::vec2 GOSprite::GetPosition() const
{
	return position;
}

void GOSprite::SetPosition(const glm::vec2& newPosition)
{
	position = newPosition;
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
	glNamedBufferSubData(positionBuffer, 0, sizeof(position), &position);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, (int)model->GetIndices().size(), GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);

	ShaderManager::EndShaderUsage("Sprite");
}

void GOSprite::Translate(const glm::vec2& translation)
{
	position += translation;
}

void GOSprite::SetTexture(Texture* const newTexture)
{
	imageTexture = newTexture;
}

glm::mat4 GOSprite::GetProjection() const
{
	return projection;
}

Texture* const GOSprite::GetTexture() const
{
	return imageTexture;
}
