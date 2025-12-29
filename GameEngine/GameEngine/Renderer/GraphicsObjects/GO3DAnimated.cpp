#include "GO3DAnimated.h"

#include "../Animation/Animation.h"
#include "../Animation/Armature.h"
#include "../../Editor/Editor.h"
#include "../Model/Model.h"
#include "../Time/TimeManager.h"
#include "../Animation/CrossFadeController.h"

const glm::mat4* const GO3DAnimated::GetAnimPoseArray() const
{
	return animationData.pose;
}

const std::vector<glm::mat4>& GO3DAnimated::GetAnimInvBindPoseArray() const
{
	return model->GetArmature()->GetInvBindPose();
}

void GO3DAnimated::Update()
{	
	if (lastFrame != TimeManager::GetFrameID())
	{
		//animation->Update(animationData.pose);
		//
		//const std::vector<glm::mat4>& invBindPose = model->GetArmature()->GetInvBindPose();
		//for (unsigned int i = 0; i < invBindPose.size(); i++)
		//{
		//	poseData[i] = animationData.pose[i] * invBindPose[i];
		//}

		
		//glNamedBufferSubData(animationBuffer, 0, sizeof(AnimationData), poseData.data());
	}

	const std::vector<glm::mat4>& invBindPose = model->GetArmature()->GetInvBindPose();
	std::vector<glm::mat4> pose(invBindPose.size());
	crossFadeController->GetCurrentPose().GetJointMatrices(pose);
	for (unsigned int i = 0; i < invBindPose.size(); i++)
	{
		poseData[i] = pose[i] * invBindPose[i];
	}

	glNamedBufferSubData(animationBuffer, 0, sizeof(AnimationData), poseData.data());

	lastFrame = TimeManager::GetFrameID();

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, animationBuffer);
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

	animation = new Animation(model->GetBakedAnimation(0));
	animation->Update(animationData.pose);

	const std::vector<glm::mat4>& invBindPose = model->GetArmature()->GetInvBindPose();
	poseData.resize(invBindPose.size());
	for (unsigned int i = 0; i < invBindPose.size(); i++)
	{
		poseData[i] = animationData.pose[i] * invBindPose[i];
	}

	glNamedBufferStorage(animationBuffer, sizeof(AnimationData), poseData.data(), GL_DYNAMIC_STORAGE_BIT);

	PauseAnimationOnEditorEnable();
}

GO3DAnimated::~GO3DAnimated()
{
	Editor::DeregisterOnEditorDisable(onEditorDisable);
	Editor::DeregisterOnEditorEnable(onEditorEnable);

	delete animation;
	delete onEditorEnable;
	delete onEditorDisable;
	glDeleteBuffers(1, &animationBuffer);

	
}

void GO3DAnimated::PauseAnimationOnEditorEnable()
{
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
