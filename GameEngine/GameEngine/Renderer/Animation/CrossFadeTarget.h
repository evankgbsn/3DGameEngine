#ifndef CROSSFADETARGET_H
#define CROSSFADETARGET_H

#include "Pose.h"
#include "Clip.h"

struct CrossFadeTarget
{
	inline CrossFadeTarget() :
		clip(0),
		time(0.0f),
		duration(0.0f),
		elapsed(0.0f)
	{
	}

	inline CrossFadeTarget(FastClip* c, const Pose& p, float dur) :
		pose(p),
		clip(c),
		time(c->GetStartTime()),
		duration(dur),
		elapsed(0.0f)
	{
	}

	Pose pose;

	FastClip* clip;

	float time;

	float duration;

	float elapsed;

};

#endif //CROSSFADETARGET_H