#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include "Pose.h"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class CrossFadeController;
class Armature;
class Model;

class AnimationController
{
public:

	AnimationController(Model* model);

	~AnimationController();

	void Update(glm::mat4* posePalette);

	void CurrentPose(glm::mat4* posePalette);

	void Play(const std::string& clipName);

	void FadeTo(const std::string& clipName, float time);

	bool Fading() const;

	void SetSpeed(float newSpeed);

	float GetSpeed() const;

	float GetSampleTime() const;

	void SetSampleTime(float sampleTime);

	void Pause();

	void Resume();

	void InitializeAdditiveAnimation(const std::string& animation);

	void SetAdditiveAnimationTime(const std::string& animation, float time);

	const std::string& GetCurrentClipName() const;

private:

	AnimationController(const AnimationController&) = delete;

	AnimationController& operator=(const AnimationController&) = delete;

	AnimationController(AnimationController&&) = delete;

	AnimationController& operator=(AnimationController&&) = delete;

	Model* model;

	CrossFadeController* crossFadeController;

	std::string currentClipName;

	float speed;

	bool fading;

	float currentFadeTime;

	bool paused;

	std::unordered_map<std::string, Pose> additiveBaseAnimations;

	std::unordered_map<std::string, Pose> additiveAnimations;
};

#endif //ANIMATIONCONTROLLER_H