#include "GOColoredAnimated.h"

#include "../Shader/ShaderManager.h"
#include "../Model/Model.h"
#include "../Animation/Animation.h"
#include "../Animation/Armature.h"

#include <GL/glew.h>

GOColoredAnimated::GOColoredAnimated(Model* const model, glm::vec4 initialColor) :
	GO3DAnimated(model),
	color(initialColor),
	colorBuffer()
{
	glCreateBuffers(1, &colorBuffer);
	glNamedBufferStorage(colorBuffer, sizeof(color), &initialColor, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &animationBuffer);
	glNamedBufferStorage(animationBuffer, sizeof(AnimationData), &animationData, GL_DYNAMIC_STORAGE_BIT);

	mvp.model = glm::mat4(1.0f);

}

GOColoredAnimated::~GOColoredAnimated()
{
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &animationBuffer);
}

void GOColoredAnimated::Update()
{
	ShaderManager::StartShaderUsage("ColoredAnimated");

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, colorBuffer);
	glNamedBufferSubData(colorBuffer, 0, sizeof(glm::vec4), &color);

	GO3DAnimated::Update();
	GO3D::Update();

	ShaderManager::EndShaderUsage("ColoredAnimated");
}

const glm::vec4& GOColoredAnimated::GetColor() const
{
	return color;
}

void GOColoredAnimated::SetColor(const glm::vec4& newColor)
{
	color = newColor;
}
