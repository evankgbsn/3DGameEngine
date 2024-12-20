#include "Animation.h"

#include "BakedAnimation.h"
#include "../../Time/TimeManager.h"

Animation::Animation(const BakedAnimation& ba) :
	playback(0.0f),
	speed(0.0f),
	bakedAnimation(ba),
	index(0U)
{
}

Animation::~Animation()
{
}

void Animation::Update(glm::mat4* posePalette)
{
	playback += TimeManager::DeltaTime();
	if (playback >= ANIMATION_PLAYBACK_FRAME_TIME)
	{
		for (unsigned int i = 0; i < bakedAnimation.GetPoseAtIndex(index).size(); ++i)
		{
			posePalette[i] = bakedAnimation.GetPoseAtIndex(index)[i];
		}

		++index *= static_cast<unsigned int>(speed);
		if (index >= bakedAnimation.GetFrameCount())
		{
			index = 0;
		}
		playback = 0;
	}
}

void Animation::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}

float Animation::GetSpeed() const
{
	return speed;
}

unsigned int Animation::GetFrame() const
{
	return index;
}

void Animation::SetFrame(unsigned int frameIndex)
{
	index = frameIndex;
}
