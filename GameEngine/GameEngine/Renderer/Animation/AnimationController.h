#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

class AnimationController
{
public:

	AnimationController();

	~AnimationController();

private:

	AnimationController(const AnimationController&) = delete;

	AnimationController& operator=(const AnimationController&) = delete;

	AnimationController(AnimationController&&) = delete;

	AnimationController& operator=(AnimationController&&) = delete;
};

#endif //ANIMATIONCONTROLLER_H