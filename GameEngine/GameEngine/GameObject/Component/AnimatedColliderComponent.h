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

	AnimatedColliderComponent();

	AnimatedColliderComponent(GraphicsObjectTexturedAnimated* const graphicsObject);

	AnimatedColliderComponent(GraphicsObjectTexturedAnimatedLit* const graphicsObject);

	AnimatedColliderComponent(GraphicsObjectColoredAnimated* const graphicsObject);

	~AnimatedColliderComponent();

	void Update() override;

	void ToggleVisibility();

	bool Intersect(const OrientedBoundingBoxComponent& other) const;

	bool IsVisible() const;

	bool Intersect(const LineSegment3D& lineSegement) const;

	bool Intersect(const StaticColliderComponent& other, glm::vec3& outHit) const;

	bool SphereIntersect(const StaticColliderComponent& other) const;

	bool BoxIntersect(const StaticColliderComponent& other) const;

	void SetGraphics(GraphicsObjectTexturedAnimated* const graphicsObject);

	void SetGraphics(GraphicsObjectTexturedAnimatedLit* const graphicsObject);

	void SetGraphics(GraphicsObjectColoredAnimated* const graphicsObject);

	glm::mat4 GetJointTransform(const std::string& jointName) const;

private:


	AnimatedColliderComponent(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent& operator=(const AnimatedColliderComponent&) = delete;

	AnimatedColliderComponent(AnimatedColliderComponent&&) = delete;

	AnimatedColliderComponent& operator=(AnimatedColliderComponent&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	AnimatedCollider* collider;

};

#endif // ANIMATEDCOLLIDERCOMPONENT_H