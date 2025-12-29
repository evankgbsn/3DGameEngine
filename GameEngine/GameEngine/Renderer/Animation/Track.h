#ifndef TRACK_H
#define TRACK_H

#include "Frame.h"

#include <vector>
#include <glm/glm.hpp>

template<typename T, size_t N>
class Track
{

public:

	Track();

	Track(const Interp& interpolationType);

	~Track();

	Track(const Track&) = default;

	Track& operator=(const Track&) = default;

	Track(Track&&) = default;

	Track& operator=(Track&&) = default;

	Frame<N>& operator[](unsigned int index);

	void SetInterpolation(Interp newInterpolation);

	Interp GetInterpolation() const;

	unsigned int Size() const;

	void SetSize(unsigned int newSize);

	const float& GetStartTime() const;

	const float& GetEndTime() const;

	T Sample(float time, bool isLooping);

	T Hermite(float time, const T& p0, const T& p1, const T& t0, const T& t1) const;

	virtual unsigned int FrameIndex(float time, bool isLooping) const;

	float AdjustTimeToFitTrack(float time, float isLooping) const;

	T Cast(float* value) const;

protected:

	std::vector<Frame<N>> frames;

private:

	T SampleConstant(float time, bool isLooping);
	
	T SampleLinear(float time, bool isLooping);
	
	T SampleCubic(float time, bool isLooping);

	Interp interpolation;
};

template<typename T, size_t N>
class FastTrack : public Track<T, N>
{

public:

	void UpdateIndexLookupTable();

protected:

	std::vector<unsigned int> sampledFrames;
	virtual unsigned int FrameIndex(float time, bool looping) const override;
};

template<typename T, size_t N>
inline void FastTrack<T, N>::UpdateIndexLookupTable()
{
	int numFrames = (int)this->frames.size();
	if (numFrames <= 1)
	{
		return;
	}

	float duration = this->GetEndTime() - this->GetStartTime();

	unsigned int numSamples = duration * 240.0f;

	sampledFrames.resize(numSamples);

	for (unsigned int i = 0; i < numSamples; ++i)
	{
		float t = (float)i / (float)(numSamples - 1);
		float time = t * duration + this->GetStartTime();

		unsigned int frameIndex = 0;

		for (int j = numFrames - 1; j >= 0; --j)
		{
			if (time >= this->frames[j].GetTime())
			{
				frameIndex = (unsigned int)j;
				if ((int)frameIndex >= numFrames - 2)
				{
					frameIndex = numFrames - 2;
				}
				break;
			}
		}
		sampledFrames[i] = frameIndex;
	}
}

template<typename T, size_t N>
inline unsigned int FastTrack<T, N>::FrameIndex(float time, bool looping) const
{
	unsigned int size = (unsigned int)this->frames.size();
	if (size <= 1)
	{
		return UINT_MAX;
	}

	if (looping)
	{
		float startTime = this->frames[0].GetTime();
		float endTime = this->frames[size - 1].GetTime();
		float duration = endTime - startTime;

		time = fmodf(time - startTime, endTime - startTime);

		if (time < 0.0f)
		{
			time += endTime + startTime;
		}

		time = time + startTime;
	}
	else
	{
		if (time <= this->frames[0].GetTime())
		{
			return 0;
		}

		if (time >= this->frames[size - 2].GetTime())
		{
			return size - 2;
		}
	}

	float duration = this->GetEndTime() - this->GetStartTime();
	float t = time / duration;
	unsigned int numSamples = duration * 240.0f;
	unsigned int index = (t * (float)numSamples);
	if (index >= sampledFrames.size())
	{
		return UINT_MAX;
	}

	return sampledFrames[index];
}

template<typename T, size_t N>
FastTrack<T, N> OptimizeTrack(Track<T, N>& input)
{
	FastTrack<T, N> result;
	result.SetInterpolation(input.GetInterpolation());
	unsigned int size = input.Size();
	result.SetSize(size);
	for (unsigned int i = 0; i < size; ++i)
	{
		result[i] = input[i];
	}
	result.UpdateIndexLookupTable();
	return result;
}

typedef Track<float, 1> ScalarTrack;
typedef Track<glm::vec3, 3> VectorTrack;
typedef Track<glm::quat, 4> QuaternionTrack;

typedef FastTrack<float, 1> ScalarFastTrack;
typedef FastTrack<glm::vec3, 3> VectorFastTrack;
typedef FastTrack<glm::quat, 4> QuaternionFastTrack;

#endif // TRACK_H