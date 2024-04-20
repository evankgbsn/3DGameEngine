#include "GOTexturedAnimated.h"

#include "../Shader/ShaderManager.h"
#include "../Texture/Texture.h"
#include "../Animation/Animation.h"
#include "../Animation/Armature.h"
#include "../Model/Model.h"

#include <GL/glew.h>

GOTexturedAnimated::GOTexturedAnimated(Model* const model, Texture* const tex) :
	GO3D(model),
	animationBuffer(),
	animationData(),
	animation(nullptr),
	texture(tex)
{
	glCreateBuffers(1, &animationBuffer);
	glNamedBufferStorage(animationBuffer, sizeof(AnimationData), &animationData, GL_DYNAMIC_STORAGE_BIT);

	animation = new Animation(model->GetBakedAnimation(0));

	for (unsigned int i = 0; i < model->GetArmature()->GetInvBindPose().size(); i++)
	{
		animationData.invBindPose[i] = model->GetArmature()->GetInvBindPose()[i];
	}
}

GOTexturedAnimated::~GOTexturedAnimated()
{
	delete animation;

	glDeleteBuffers(1, &animationBuffer);
}

void GOTexturedAnimated::Update()
{
	ShaderManager::StartShaderUsage("TexturedAnimated");

	texture->Bind(GL_TEXTURE0);

	animation->Update(animationData.pose);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, animationBuffer);
	glNamedBufferSubData(animationBuffer, 0, sizeof(AnimationData), &animationData);

	GO3D::Update();

	ShaderManager::EndShaderUsage("TexturedAnimated");
}
