#ifndef CROSSFADECONTROLLER_H
#define CROSSFADECONTROLLER_H

#include "CrossFadeTarget.h"
#include "Armature.h"

#include <vector>

class CrossFadeController
{
public:

	CrossFadeController();

	~CrossFadeController();

	CrossFadeController(const Armature& armature);

	void SetArmature(const Armature& armature);

	void Play(FastClip* target);

	void FadeTo(FastClip* target, float fadeTime);

	void Update(float deltaTime);

	const Pose& GetCurrentPose() const;

	FastClip* GetCurrentClip() const;

protected:

	std::vector<CrossFadeTarget> targets;

	FastClip* clip;

	float time;

	Pose pose;

	Armature armature;

	bool wasArmatureSet;

private:

	CrossFadeController(const CrossFadeController&) = default;

	CrossFadeController& operator=(const CrossFadeController&) = default;

	CrossFadeController(CrossFadeController&&) = default;

	CrossFadeController& operator=(CrossFadeController&&) = default;
};

#endif //CROSSFADECONTROLLER_H