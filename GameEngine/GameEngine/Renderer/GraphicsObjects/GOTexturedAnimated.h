#ifndef GOTEXTUREDANIMATED_H
#define GOTEXTUREDANIMATED_H

#include "GO3D.h"

class Texture;
class Animation;

class GOTexturedAnimated : public GO3D
{

public:

	GOTexturedAnimated(Model* const model, Texture* const texture);

	~GOTexturedAnimated();

	void Update() override;

private:

	GOTexturedAnimated(const GOTexturedAnimated&) = delete;

	GOTexturedAnimated& operator=(const GOTexturedAnimated&) = delete;

	GOTexturedAnimated(GOTexturedAnimated&&) = delete;

	GOTexturedAnimated& operator=(GOTexturedAnimated&&) = delete;

	struct AnimationData
	{
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
	} animationData;

	unsigned int animationBuffer;

	Animation* animation;

	Texture* texture;

};

#endif // GOTEXTUREDANIMATED_H