#include "Clip.h"

#include "Pose.h"

template TClip<TransformTrack>;
template TClip<FastTransformTrack>;

template<typename TRACK>
TClip<TRACK>::TClip() :
    name("No name given"),
    startTime(0.0f),
    endTime(0.0f),
    isLooping(true)
{
}

template<typename TRACK>
TClip<TRACK>::~TClip()
{
}

template<typename TRACK>
TRACK& TClip<TRACK>::operator[](unsigned int joint)
{
    for (unsigned int i = 0; i < tracks.size(); i++)
    {
        if (tracks[i].GetId() == joint)
        {
            return tracks[i];
        }
    }

    tracks.push_back(TRACK());
    tracks[tracks.size() - 1].SetId(joint);
    return tracks[tracks.size() - 1];
}

template<typename TRACK>
unsigned int TClip<TRACK>::GetIdAtIndex(unsigned int index) const
{
    return tracks[index].GetId();
}

template<typename TRACK>
void TClip<TRACK>::SetIdAtIndex(unsigned int index, unsigned int id)
{
    tracks[index].SetId(id);
}

template<typename TRACK>
unsigned int TClip<TRACK>::Size() const
{
    return tracks.size();
}

template<typename TRACK>
float TClip<TRACK>::Sample(Pose& outPose, float time)
{
    if (GetDuration() == 0.0f)
    {
        return 0.0f;
    }

    time = AdjustTimeToFitRange(time);

    unsigned int size = static_cast<unsigned int>(tracks.size());
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned int j = tracks[i].GetId(); // Joint
        Math::Transform local = outPose.GetLocalTransform(j);
        Math::Transform animated = tracks[i].Sample(local, time, isLooping);
        outPose.SetLocalTransform(j,animated);
    }
    return time;
}

template<typename TRACK>
void TClip<TRACK>::RecalculateDuration()
{
    startTime = 0;
    endTime = 0;
    bool startSet = false;
    bool endSet = false;

    unsigned int tracksSize = static_cast<unsigned int>(tracks.size());
    for (unsigned int i = 0; i < tracksSize; i++)
    {
        if (tracks[i].IsValid())
        {
            float st = tracks[i].GetStartTime();
            float et = tracks[i].GetEndTime();

            if (st < startTime || !startSet)
            {
                startTime = st;
                startSet = true;
            }

            if (et > endTime || !endSet)
            {
                endTime = et;
                endSet = true;
            }
        }
    }
}

template<typename TRACK>
const std::string& TClip<TRACK>::GetName() const
{
    return name;
}

template<typename TRACK>
void TClip<TRACK>::SetName(const std::string& n)
{
    name = n;
}

template<typename TRACK>
float TClip<TRACK>::GetDuration() const
{
    return endTime - startTime;
}

template<typename TRACK>
float TClip<TRACK>::GetStartTime() const
{
    return startTime;
}

template<typename TRACK>
float TClip<TRACK>::GetEndTime() const
{
    return endTime;
}

template<typename TRACK>
bool TClip<TRACK>::IsLooping() const
{
    return isLooping;
}

template<typename TRACK>
void TClip<TRACK>::SetIsLooping(const bool& loop)
{
    isLooping = loop;
}

template<typename TRACK>
float TClip<TRACK>::AdjustTimeToFitRange(float time) const
{
    if (isLooping)
    {
        float duration = endTime - startTime;
        if (duration <= 0)
        {
            return 0.0f;
        }

        time = fmod(time - startTime, duration);

        if (time < 0)
        {
            time += duration;
        }

        time += startTime;
    }
    else
    {
        if (time < startTime)
        {
            time = startTime;
        }
        else if (time > endTime)
        {
            time = endTime;
        }
    }

    return time;
}


FastClip OptimizeClip(Clip& input)
{
    FastClip result;

    result.SetName(input.GetName());
    result.SetIsLooping(input.IsLooping());
    unsigned int size = input.Size();
    for (unsigned int i = 0; i < size; ++i)
    {
        unsigned int joint = input.GetIdAtIndex(i);
        result[joint] = OptimizeTransformTrack(input[joint]);
    }
    result.RecalculateDuration();
    return result;
}