#include "GO3DAnimated.h"

#include "../Animation/Animation.h"
#include "../Animation/Armature.h"
#include "../../Editor/Editor.h"
#include "../Model/Model.h"

glm::mat4* GO3DAnimated::GetAnimPoseArray()
{
	return animationData.pose;
}

glm::mat4* GO3DAnimated::GetAnimInvBindPoseArray()
{
	return animationData.invBindPose;
}

void GO3DAnimated::Update()
{	
	animation->Update(animationData.pose);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, animationBuffer);
	glNamedBufferSubData(animationBuffer, 0, sizeof(AnimationData), &animationData);
}

void GO3DAnimated::SetClip(unsigned int clipIndex)
{
	delete animation;
	animation = new Animation(model->GetBakedAnimation(clipIndex));
	clip = clipIndex;
}

unsigned int GO3DAnimated::GetClip() const
{
	return clip;
}

unsigned int GO3DAnimated::GetFrame() const
{
	return animation->GetFrame();
}

void GO3DAnimated::SetFrame(unsigned int frameIndex)
{
	animation->SetFrame(frameIndex);
}

float GO3DAnimated::GetSpeed() const
{
	return speed;
}

void GO3DAnimated::SetSpeed(float spd)
{
	speed = spd;

	if (!Editor::IsEnabled())
	{
		animation->SetSpeed(speed);
	}
}

void GO3DAnimated::PauseAnimation()
{
	animation->SetSpeed(0.0f);
}

void GO3DAnimated::ResumeAnimation()
{
	SetSpeed(speed);
}

GO3DAnimated::GO3DAnimated(Model* const model) :
	GO3D(model),
	animationData(),
	animationBuffer(),
	clip(0)
{
	glCreateBuffers(1, &animationBuffer);
	glNamedBufferStorage(animationBuffer, sizeof(AnimationData), &animationData, GL_DYNAMIC_STORAGE_BIT);

	animation = new Animation(model->GetBakedAnimation(0));

	PauseAnimationOnEditorEnable();
}

GO3DAnimated::~GO3DAnimated()
{

	if (Editor::IsEnabled())
	{
		Editor::DeregisterOnEditorDisable(onEditorDisable);
		Editor::DeregisterOnEditorEnable(onEditorEnable);
	}

	delete animation;
	delete onEditorEnable;
	delete onEditorDisable;
	glDeleteBuffers(1, &animationBuffer);

	
}

void GO3DAnimated::PauseAnimationOnEditorEnable()
{
	for (unsigned int i = 0; i < model->GetArmature()->GetInvBindPose().size(); i++)
	{
		animationData.invBindPose[i] = model->GetArmature()->GetInvBindPose()[i];
	}

	onEditorEnable = new std::function<void()>([this]()
		{
			PauseAnimation();

		});

	onEditorDisable = new std::function<void()>([this]()
		{
			ResumeAnimation();
		});

	Editor::RegisterOnEditorEnable(onEditorEnable);
	Editor::RegisterOnEditorDisable(onEditorDisable);
}
