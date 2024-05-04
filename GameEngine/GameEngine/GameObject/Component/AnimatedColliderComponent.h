#ifndef ANIMATEDCOLLIDERCOMPONENT_H
#define ANIMATEDCOLLIDERCOMPONENT_H

#include "Component.h"

class AnimatedCollider;

class AnimatedColliderComponent : public Component
{

public:

	AnimatedColliderComponent();

	~AnimatedColliderComponent();

private:


	AnimatedColliderComponent(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent& operator=(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent(AnimatedColliderComponent&&) = delete;

	AnimatedColliderComponent& operator=(AnimatedColliderComponent&&) = delete;

	AnimatedCollider* collider;

};

#endif // ANIMATEDCOLLIDERCOMPONENT_H