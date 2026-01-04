#ifndef GO3DANIMATED_H
#define GO3DANIMATED_H

#include "GO3D.h"

#include <functional>
#include <vector>
#include <string>

class AnimationController;

class GO3DAnimated : public GO3D
{
public:

	const glm::mat4* const GetAnimPoseArray() const;

	const std::vector<glm::mat4>& GetAnimInvBindPoseArray() const;

	virtual void Update() override;

	void SetClip(const std::string& clipName);

	const std::string& GetClipName() const;

	float GetSampleTime() const;

	void FadeTo(const std::string& clipName, float time);

	void SetSampleTime(float sampleTime);

	float GetSpeed() const;

	void SetSpeed(float speed);

	void PauseAnimation();

	void ResumeAnimation();

	void InitializeAdditiveAnimation(const std::string& animation);

	void SetAdditiveAnimationTime(const std::string& animation, float time);

	void GetPose(std::vector<glm::mat4>& pose) const;

	void SetPose(std::vector<glm::mat4>& pose);

	void ClearSetPose();

protected:

	GO3DAnimated(Model* const model);

	virtual ~GO3DAnimated();

	struct AnimationData
	{
		glm::mat4 pose[500];
	} animationData;

	unsigned int animationBuffer;

private:

	GO3DAnimated(const GO3DAnimated&) = delete;

	GO3DAnimated& operator=(const GO3DAnimated&) = delete;

	GO3DAnimated(GO3DAnimated&&) = delete;

	GO3DAnimated& operator=(GO3DAnimated&&) = delete;

	void PauseAnimationOnEditorEnable();

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	AnimationController* animationController;

	unsigned long long currentFrame;

	bool poseSet = false;

};

#endif // GO3DANIMATED_H