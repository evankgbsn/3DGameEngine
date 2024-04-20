#ifndef GOCOLOREDANIMATED_H
#define GOCOLOREDANIMATED_H

#include "GO3D.h"

#include <glm/glm.hpp>

class Animation;

class GOColoredAnimated : public GO3D
{
public:

	GOColoredAnimated(Model* const model, glm::vec4 initialColor);

	~GOColoredAnimated();

	void Update() override;

protected:

private:

	GOColoredAnimated(const GOColoredAnimated&) = delete;

	GOColoredAnimated& operator=(const GOColoredAnimated&) = delete;
	
	GOColoredAnimated(GOColoredAnimated&&) = delete;

	GOColoredAnimated& operator=(GOColoredAnimated&&) = delete;

	glm::vec4 color;

	unsigned int colorBuffer;

	struct AnimationData
	{
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
	} animationData;

	unsigned int animationBuffer;

	Animation* animation;
};

#endif // GOCOLOREDANIMATED_H