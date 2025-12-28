#ifndef GO3DANIMATED_H
#define GO3DANIMATED_H

#include "GO3D.h"

#include <functional>
#include <vector>

class Animation;

class GO3DAnimated : public GO3D
{
public:

	const glm::mat4* const GetAnimPoseArray() const;

	const std::vector<glm::mat4>& GetAnimInvBindPoseArray() const;

	virtual void Update() override;

	void SetClip(unsigned int clipIndex);

	unsigned int GetClip() const;

	unsigned int GetFrame() const;

	void SetFrame(unsigned int frameIndex);

	float GetSpeed() const;

	void SetSpeed(float speed);

	void PauseAnimation();

	void ResumeAnimation();

protected:

	GO3DAnimated(Model* const model);

	virtual ~GO3DAnimated();

	struct AnimationData
	{
		glm::mat4 pose[500];
	} animationData;

	unsigned int animationBuffer;

	Animation* animation;

private:

	GO3DAnimated(const GO3DAnimated&) = delete;

	GO3DAnimated& operator=(const GO3DAnimated&) = delete;

	GO3DAnimated(GO3DAnimated&&) = delete;

	GO3DAnimated& operator=(GO3DAnimated&&) = delete;

	void PauseAnimationOnEditorEnable();

	unsigned int clip;

	float speed;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	unsigned long long lastFrame;

};

#endif // GO3DANIMATED_H