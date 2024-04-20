#include "GOColoredAnimated.h"

#include "../Shader/ShaderManager.h"
#include "../Model/Model.h"
#include "../Animation/Animation.h"
#include "../Animation/Armature.h"

#include <GL/glew.h>

GOColoredAnimated::GOColoredAnimated(Model* const model, glm::vec4 initialColor) :
	GO3D(model),
	color(initialColor),
	colorBuffer(),
	animationData(),
	animationBuffer(),
	animation(nullptr)
{
	glCreateBuffers(1, &colorBuffer);
	glNamedBufferStorage(colorBuffer, sizeof(color), &color, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &animationBuffer);
	glNamedBufferStorage(animationBuffer, sizeof(AnimationData), &animationData, GL_DYNAMIC_STORAGE_BIT);

	animation = new Animation(model->GetBakedAnimation(0));

	for (unsigned int i = 0; i < model->GetArmature()->GetInvBindPose().size(); i++)
	{
		animationData.invBindPose[i] = model->GetArmature()->GetInvBindPose()[i];
	}

	mvp.model = glm::mat4(1.0f);

}

GOColoredAnimated::~GOColoredAnimated()
{
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &animationBuffer);
	delete animation;
}

void GOColoredAnimated::Update()
{
	ShaderManager::StartShaderUsage("ColoredAnimated");

	animation->Update(animationData.pose);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, animationBuffer);

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, colorBuffer);

	glNamedBufferSubData(animationBuffer, 0, sizeof(AnimationData), &animationData);
	glNamedBufferSubData(colorBuffer, 0, sizeof(glm::vec4), &color);

	GO3D::Update();

	ShaderManager::EndShaderUsage("ColoredAnimated");
}
