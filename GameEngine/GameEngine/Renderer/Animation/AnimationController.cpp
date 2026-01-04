#include "AnimationController.h"

#include "Clip.h"
#include "CrossFadeController.h"
#include "../Model/Model.h"
#include "../../Time/TimeManager.h"

AnimationController::AnimationController(Model* m) :
	model(m),
	crossFadeController(new CrossFadeController(*model->GetArmature())),
	speed(1.0f),
	currentClipName(""),
	fading(false),
	paused(false),
	currentFadeTime(0.0f)
{
}

AnimationController::~AnimationController()
{
	delete crossFadeController;
}

void AnimationController::Update(glm::mat4* posePalette)
{
	if (paused)
	{

	}
	else
	{
		currentFadeTime -= TimeManager::DeltaTime();

		if (currentFadeTime <= 0.0f)
		{
			fading = false;
		}
	}

	crossFadeController->Update(TimeManager::DeltaTime(), speed);

	Pose pose = crossFadeController->GetCurrentPose();

	for (auto& additivePose : additiveAnimations)
	{
		Pose::Add(pose, pose, additivePose.second, additiveBaseAnimations[additivePose.first], -1);
	}

	std::vector<glm::mat4> joints;
	pose.GetJointMatrices(joints);

	for (unsigned int i = 0; i < joints.size(); i++)
	{
		posePalette[i] = joints[i];
	}
}

void AnimationController::CurrentPose(glm::mat4* posePalette)
{
	Pose pose = crossFadeController->GetCurrentPose();

	for (auto& additivePose : additiveAnimations)
	{
		Pose::Add(pose, pose, additivePose.second, additiveBaseAnimations[additivePose.first], -1);
	}

	std::vector<glm::mat4> joints;
	pose.GetJointMatrices(joints);

	for (unsigned int i = 0; i < joints.size(); i++)
	{
		posePalette[i] = joints[i];
	}
}

void AnimationController::Play(const std::string& clipName)
{
	const std::vector<FastClip>& clips = model->GetAnimationClips();

	unsigned int index = 0;
	for (index; index < clips.size(); ++index)
	{
		if (clips[index].GetName() == clipName)
		{
			break;
		}
	}

	currentClipName = clipName;

	crossFadeController->Play(model->GetAnimationClip(index));
}

void AnimationController::FadeTo(const std::string& clipName, float time)
{
	const std::vector<FastClip>& clips = model->GetAnimationClips();

	unsigned int index = 0;
	for (index; index < clips.size(); ++index)
	{
		if (clips[index].GetName() == clipName)
		{
			break;
		}
	}

	crossFadeController->FadeTo(model->GetAnimationClip(index), time);

	fading = true;
	currentFadeTime = time;
	currentClipName = clipName;
}

bool AnimationController::Fading() const
{
	return fading;
}

void AnimationController::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}

float AnimationController::GetSpeed() const
{
	return speed;
}

float AnimationController::GetSampleTime() const
{
	return crossFadeController->GetTime();
}

void AnimationController::SetSampleTime(float newTime)
{
	crossFadeController->SetTime(newTime);
}

void AnimationController::Pause()
{
	paused = true;
}

void AnimationController::Resume()
{
	paused = false;
}

void AnimationController::InitializeAdditiveAnimation(const std::string& animation)
{
	const std::vector<FastClip>& clips = model->GetAnimationClips();

	unsigned int index = 0;
	for (index; index < clips.size(); ++index)
	{
		if (clips[index].GetName() == animation)
		{
			break;
		}
	}

	FastClip* clip = model->GetAnimationClip(index);

	if (clip != nullptr)
	{
		additiveBaseAnimations[animation] = Pose::MakeAdditivePose(*model->GetArmature(), *clip);
		additiveAnimations[animation] = model->GetArmature()->GetRestPose();

		clip->SetIsLooping(false);
	}
}

void AnimationController::SetAdditiveAnimationTime(const std::string& animation, float time)
{
	const std::vector<FastClip>& clips = model->GetAnimationClips();

	unsigned int index = 0;
	for (index; index < clips.size(); ++index)
	{
		if (clips[index].GetName() == animation)
		{
			break;
		}
	}

	FastClip* clip = model->GetAnimationClip(index);

	if (clip != nullptr)
	{
		float t = clip->GetStartTime() + (clip->GetDuration() * time);
		clip->Sample(additiveAnimations[animation], t);
	}
}

const std::string& AnimationController::GetCurrentClipName() const
{
	return currentClipName;
}
