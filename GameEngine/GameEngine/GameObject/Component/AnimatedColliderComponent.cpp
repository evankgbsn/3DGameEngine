#include "AnimatedColliderComponent.h"

#include "../../Collision/AnimatedCollider.h"
#include "GraphicsObjectColoredAnimated.h"
#include "GraphicsObjectTexturedAnimated.h"
#include "GraphicsObjectTexturedAnimatedLit.h"
#include "../../Math/Shapes/LineSegment3D.h"
#include "StaticColliderComponent.h"
#include "OrientedBoundingBoxComponent.h"
#include "../../Collision/OrientedBoundingBoxWithVisualization.h"

AnimatedColliderComponent::AnimatedColliderComponent()
{
	RegisterComponentClassType<AnimatedColliderComponent>(this);
}

AnimatedColliderComponent::AnimatedColliderComponent(GraphicsObjectTexturedAnimated* const graphicsObject)
{
	RegisterComponentClassType<AnimatedColliderComponent>(this);
	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

AnimatedColliderComponent::AnimatedColliderComponent(GraphicsObjectTexturedAnimatedLit* const graphicsObject)
{
	RegisterComponentClassType<AnimatedColliderComponent>(this);
	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

AnimatedColliderComponent::AnimatedColliderComponent(GraphicsObjectColoredAnimated* const graphicsObject)
{
	RegisterComponentClassType<AnimatedColliderComponent>(this);
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

void AnimatedColliderComponent::SetGraphics(GraphicsObjectTexturedAnimated* const graphicsObject)
{
	if (collider != nullptr)
	{
		delete collider;
	}

	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

void AnimatedColliderComponent::SetGraphics(GraphicsObjectTexturedAnimatedLit* const graphicsObject)
{
	if (collider != nullptr)
	{
		delete collider;
	}

	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

void AnimatedColliderComponent::SetGraphics(GraphicsObjectColoredAnimated* const graphicsObject)
{
	if (collider != nullptr)
	{
		delete collider;
	}

	collider = new AnimatedCollider(static_cast<GO3DAnimated*>((void*)graphicsObject->GetGraphics()));
}

void AnimatedColliderComponent::Serialize()
{
	savedBools["IsVisible"] = collider->IsVisible();
}

void AnimatedColliderComponent::Deserialize()
{
	if (!savedBools["IsVisible"])
	{
		collider->ToggleVisibility();
	}
}
