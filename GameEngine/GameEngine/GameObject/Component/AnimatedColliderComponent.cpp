#include "AnimatedColliderComponent.h"

#include "../../Collision/AnimatedCollider.h"
#include "GraphicsObjectColoredAnimated.h"
#include "GraphicsObjectTexturedAnimated.h"
#include "GraphicsObjectTexturedAnimatedLit.h"
#include "../../Math/Shapes/LineSegment3D.h"
#include "StaticColliderComponent.h"
#include "OrientedBoundingBoxComponent.h"
#include "../../Collision/OrientedBoundingBoxWithVisualization.h"

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

bool AnimatedColliderComponent::Intersect(const OrientedBoundingBoxComponent& other) const
{
	return collider->Intersect(*other.GetCollider());
}

bool AnimatedColliderComponent::IsVisible() const
{
	return collider->IsVisible();
}

bool AnimatedColliderComponent::Intersect(const LineSegment3D& lineSegment) const
{
	return collider->Intersect(lineSegment);
}

bool AnimatedColliderComponent::Intersect(const StaticColliderComponent& other) const
{
	return collider->Intersect(*other.GetCollider());
}

const std::vector<char> AnimatedColliderComponent::Serialize() const
{
	return std::vector<char>();
}

void AnimatedColliderComponent::Deserialize(const std::vector<char>& data)
{
}
