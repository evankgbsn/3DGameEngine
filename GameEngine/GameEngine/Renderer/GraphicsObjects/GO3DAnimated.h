#ifndef GO3DANIMATED_H
#define GO3DANIMATED_H

#include "GO3D.h"

class Animation;

class GO3DAnimated : public GO3D
{
public:

	glm::mat4* GetAnimPoseArray();

	glm::mat4* GetAnimInvBindPoseArray();

	virtual void Update() override;

	void SetClip(unsigned int clipIndex);

protected:

	GO3DAnimated(Model* const model);

	virtual ~GO3DAnimated();

private:

	GO3DAnimated(const GO3DAnimated&) = delete;

	GO3DAnimated& operator=(const GO3DAnimated&) = delete;

	GO3DAnimated(GO3DAnimated&&) = delete;

	GO3DAnimated& operator=(GO3DAnimated&&) = delete;

	Animation* animation;

	struct AnimationData
	{
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
	} animationData;

	unsigned int animationBuffer;

};

#endif // GO3DANIMATED_H