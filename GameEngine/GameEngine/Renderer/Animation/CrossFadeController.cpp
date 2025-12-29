#include "CrossFadeController.h"

CrossFadeController::CrossFadeController() :
    clip(nullptr),
    time(0.0f),
    wasArmatureSet(false)
{
}

CrossFadeController::~CrossFadeController()
{
}

CrossFadeController::CrossFadeController(const Armature& arm) :
    clip(nullptr),
    time(0.0f)
{
    SetArmature(arm);
}

void CrossFadeController::SetArmature(const Armature& arm)
{
    armature = arm;
    pose = armature.GetRestPose();
    wasArmatureSet = true;
}

void CrossFadeController::Play(FastClip* target)
{
    targets.clear();
    clip = target;
    pose = armature.GetRestPose();
    time = target->GetStartTime();
}

void CrossFadeController::SetTime(float newTime)
{
    Play(clip);
    time = 0.0f;
    Update(newTime, 1.0f);
}

float CrossFadeController::GetTime() const
{
    return time;
}

void CrossFadeController::FadeTo(FastClip* target, float fadeTime)
{
    if (clip == nullptr)
    {
        Play(target);
        return;
    }

    if (targets.size() >= 1)
    {
        FastClip* c = targets[targets.size() - 1].clip;
        if (c == target)
        {
            return;
        }
    }
    else
    {
        if (clip == target)
        {
            return;
        }
    }

    targets.push_back(CrossFadeTarget(target, armature.GetRestPose(), fadeTime));
}

void CrossFadeController::Update(float deltaTime, float speed)
{
    if (clip == nullptr || !wasArmatureSet)
    {
        return;
    }

    unsigned int numTargets = targets.size();

    for (unsigned int i = 0; i < numTargets; ++i)
    {
        float dur = targets[i].duration;
        if (targets[i].elapsed >= dur)
        {
            clip = targets[i].clip;
            time = targets[i].time;
            pose = targets[i].pose;
            targets.erase(targets.begin() + i);
            break;
        }
    }

    numTargets = targets.size();
    pose = armature.GetRestPose();
    time = clip->Sample(pose, time + deltaTime * speed);

    for (unsigned int i = 0; i < numTargets; ++i)
    {
        CrossFadeTarget& t = targets[i];
        t.time = t.clip->Sample(t.pose, t.time + deltaTime * speed);

        t.elapsed += deltaTime;
        float curT = t.elapsed / t.duration;
        if (curT > 1.0f)
        {
            curT = 1.0f;
        }

        Pose::Blend(pose, pose, t.pose, curT, -1);
    }
}

const Pose& CrossFadeController::GetCurrentPose() const
{
    return pose;
}

FastClip* CrossFadeController::GetCurrentClip() const
{
    return clip;
}
