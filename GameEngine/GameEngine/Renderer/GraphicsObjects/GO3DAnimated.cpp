#include "GO3DAnimated.h"

#include "../Animation/AnimationController.h"
#include "../../Editor/Editor.h"
#include "../Model/Model.h"
#include "../Time/TimeManager.h"
#include "../Animation/Armature.h"

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
	const std::vector<glm::mat4>& invBindPose = model->GetArmature()->GetInvBindPose();
	std::vector<glm::mat4> pose(invBindPose.size());

	animationController->Update(pose.data());

	for (unsigned int i = 0; i < invBindPose.size(); i++)
	{
		poseData[i] = pose[i] * invBindPose[i];
	}

	glNamedBufferSubData(animationBuffer, 0, sizeof(AnimationData), poseData.data());

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, animationBuffer);
}

void GO3DAnimated::SetClip(const std::string& clipName)
{
	animationController->Play(clipName);
}

const std::string& GO3DAnimated::GetClipName() const
{
	return animationController->GetCurrentClipName();
}

float GO3DAnimated::GetSampleTime() const
{
	return animationController->GetSampleTime();
}

void GO3DAnimated::SetSampleTime(float time)
{
	animationController->SetSampleTime(time);
}

float GO3DAnimated::GetSpeed() const
{
	return animationController->GetSpeed();
}

void GO3DAnimated::SetSpeed(float spd)
{
	animationController->SetSpeed(spd);
}

void GO3DAnimated::PauseAnimation()
{
	animationController->Pause();
}

void GO3DAnimated::ResumeAnimation()
{
	animationController->Resume();
}

GO3DAnimated::GO3DAnimated(Model* const model) :
	GO3D(model),
	animationData(),
	animationBuffer()
{
	glCreateBuffers(1, &animationBuffer);

	animationController = new AnimationController(model);
	poseData.resize(model->GetArmature()->GetInvBindPose().size());

	glNamedBufferStorage(animationBuffer, sizeof(AnimationData), poseData.data(), GL_DYNAMIC_STORAGE_BIT);

	PauseAnimationOnEditorEnable();
}

GO3DAnimated::~GO3DAnimated()
{
	Editor::DeregisterOnEditorDisable(onEditorDisable);
	Editor::DeregisterOnEditorEnable(onEditorEnable);

	delete animationController;
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
