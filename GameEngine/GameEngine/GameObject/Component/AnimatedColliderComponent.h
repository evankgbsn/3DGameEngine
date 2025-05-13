#ifndef ANIMATEDCOLLIDERCOMPONENT_H
#define ANIMATEDCOLLIDERCOMPONENT_H

#include "Component.h"

class AnimatedCollider;
class StaticColliderComponent;
class OrientedBoundingBoxComponent;
class GraphicsObjectTexturedAnimated;
class GraphicsObjectTexturedAnimatedLit;
class GraphicsObjectColoredAnimated;
class LineSegment3D;

class AnimatedColliderComponent : public Component
{

public:

	AnimatedColliderComponent() = default;

	AnimatedColliderComponent(GraphicsObjectTexturedAnimated* const graphicsObject);

	AnimatedColliderComponent(GraphicsObjectTexturedAnimatedLit* const graphicsObject);

	AnimatedColliderComponent(GraphicsObjectColoredAnimated* const graphicsObject);

	~AnimatedColliderComponent();

	void Update() override;

	void ToggleVisibility();

	bool Intersect(const OrientedBoundingBoxComponent& other) const;

	bool IsVisible() const;

	bool Intersect(const LineSegment3D& lineSegement) const;

	bool Intersect(const StaticColliderComponent& other) const;

private:


	AnimatedColliderComponent(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent& operator=(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent(AnimatedColliderComponent&&) = delete;

	AnimatedColliderComponent& operator=(AnimatedColliderComponent&&) = delete;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	AnimatedCollider* collider;

};

#endif // ANIMATEDCOLLIDERCOMPONENT_H