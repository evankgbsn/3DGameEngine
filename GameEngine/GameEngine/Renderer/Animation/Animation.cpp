#include "Animation.h"

#include "BakedAnimation.h"
#include "../../Time/TimeManager.h"

Animation::Animation(const BakedAnimation& ba) :
	playback(ANIMATION_PLAYBACK_FRAME_TIME),
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
    // Accumulate time as before
    playback += TimeManager::DeltaTime() * speed;

    // Use a while loop to process all accumulated time
    while (playback >= ANIMATION_PLAYBACK_FRAME_TIME)
    {
        // Update the pose palette for the current frame
        for (unsigned int i = 0; i < bakedAnimation.GetPoseAtIndex(index).size(); ++i)
        {
            posePalette[i] = bakedAnimation.GetPoseAtIndex(index)[i];
        }

        // Advance to the next frame
        ++index;
        if (index >= bakedAnimation.GetFrameCount())
        {
            index = 0;
        }

        // Subtract the time for one frame, keeping the remainder
        playback -= ANIMATION_PLAYBACK_FRAME_TIME;
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
