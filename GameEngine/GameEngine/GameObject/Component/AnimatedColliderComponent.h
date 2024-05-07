#ifndef ANIMATEDCOLLIDERCOMPONENT_H
#define ANIMATEDCOLLIDERCOMPONENT_H

#include "Component.h"

class AnimatedCollider;
class OrientedBoundingBox;
class GraphicsObjectTexturedAnimated;
class GraphicsObjectTexturedAnimatedLit;
class GraphicsObjectColoredAnimated;

class AnimatedColliderComponent : public Component
{

public:

	AnimatedColliderComponent(GraphicsObjectTexturedAnimated* const graphicsObject);

	AnimatedColliderComponent(GraphicsObjectTexturedAnimatedLit* const graphicsObject);

	AnimatedColliderComponent(GraphicsObjectColoredAnimated* const graphicsObject);

	~AnimatedColliderComponent();

	void Update() override;

	void ToggleVisibility();

	bool Intersect(const OrientedBoundingBox& other) const;

private:


	AnimatedColliderComponent(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent& operator=(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent(AnimatedColliderComponent&&) = delete;

	AnimatedColliderComponent& operator=(AnimatedColliderComponent&&) = delete;

	AnimatedCollider* collider;

};

#endif // ANIMATEDCOLLIDERCOMPONENT_H