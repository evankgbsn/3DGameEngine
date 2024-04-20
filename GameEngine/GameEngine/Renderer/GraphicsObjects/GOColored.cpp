#include "GOColored.h"

#include "../Shader/ShaderManager.h"

#include <GL/glew.h>

GOColored::GOColored(Model* const model, const glm::vec4& initialColor) :
	GO3D(model),
	color(initialColor),
	colorBuffer(0)
{
	glCreateBuffers(1, &colorBuffer);
	glNamedBufferStorage(colorBuffer, sizeof(color), &color, GL_DYNAMIC_STORAGE_BIT);
}

GOColored::~GOColored()
{
	glDeleteBuffers(1, &colorBuffer);
}

void GOColored::SetColor(const glm::vec4& c)
{
	color = c;

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, colorBuffer);
	glNamedBufferSubData(colorBuffer, 0, sizeof(color), &color);
}

const glm::vec4& GOColored::GetColor() const
{
	return color;
}

void GOColored::Update()
{
	ShaderManager::StartShaderUsage("Test");

	GO3D::Update();

	ShaderManager::EndShaderUsage("Test");

}
