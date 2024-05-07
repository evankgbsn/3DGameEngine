#include "AnimatedColliderComponent.h"

#include "../../Collision/AnimatedCollider.h"
#include "GraphicsObjectColoredAnimated.h"
#include "GraphicsObjectTexturedAnimated.h"
#include "GraphicsObjectTexturedAnimatedLit.h"

AnimatedColliderComponent::AnimatedColliderComponent(GraphicsObjectTexturedAnimated* const graphicsObject)
{
	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

AnimatedColliderComponent::AnimatedColliderComponent(GraphicsObjectTexturedAnimatedLit* const graphicsObject)
{
	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

AnimatedColliderComponent::AnimatedColliderComponent(GraphicsObjectColoredAnimated* const graphicsObject)
{
	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

AnimatedColliderComponent::~AnimatedColliderComponent()
{
	delete collider;
}

void AnimatedColliderComponent::Update()
{
	collider->Update();
}

void AnimatedColliderComponent::ToggleVisibility()
{
	collider->ToggleVisibility();
}

bool AnimatedColliderComponent::Intersect(const OrientedBoundingBox& other) const
{
	return false;
}
