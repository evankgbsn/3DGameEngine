#ifndef CLIP_H
#define CLIP_H

#include "TransformTrack.h"

#include <vector>
#include <string>

class Pose;

template<typename TRACK>
class TClip
{

public:

	TClip();

	~TClip();

	TClip(const TClip&) = default;

	TClip& operator=(const TClip&) = default;

	TClip(TClip&&) = default;

	TClip& operator=(TClip&&) = default;

	TRACK& operator[](unsigned int joint);

	unsigned int GetIdAtIndex(unsigned int index) const;

	void SetIdAtIndex(unsigned int index, unsigned int id);

	unsigned int Size() const;

	float Sample(Pose& outPose, float time);

	void RecalculateDuration();

	const std::string& GetName() const;

	void SetName(const std::string& name);

	float GetDuration() const;

	float GetStartTime() const;

	float GetEndTime() const;

	bool IsLooping() const;

	void SetIsLooping(const bool& loop);

	float AdjustTimeToFitRange(float time) const;

private:

	std::string name;

	std::vector<TRACK> tracks;

	float startTime;

	float endTime;

	bool isLooping;
};

typedef TClip<TransformTrack> Clip;
typedef TClip<FastTransformTrack> FastClip;

FastClip OptimizeClip(Clip& input);


#endif // CLIP_H