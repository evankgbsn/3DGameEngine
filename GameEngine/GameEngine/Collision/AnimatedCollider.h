#ifndef ANIMATEDCOLLIDER_H
#define ANIMATEDCOLLIDER_H

#include "Collider.h"

#include <unordered_map>
#include <vector>
#include <string>

class GO3DAnimated;
class ColoredStaticGraphicsObject;
class OrientedBoundingBoxWithVisualization;
class OrientedBoundingBox;
class SphereWithVisualization;

class AnimatedCollider : public Collider
{
public:

	AnimatedCollider(GO3DAnimated* const graphicsObject);

	~AnimatedCollider();

	void Update() override;

	void ToggleVisibility() override;

	bool Intersect(const OrientedBoundingBox& other) const;

private:

	void InitializeOBBs();

	void InitializeSphere();

	AnimatedCollider() = delete;

	AnimatedCollider(const AnimatedCollider&) = delete;

	AnimatedCollider& operator=(const AnimatedCollider) = delete;

	AnimatedCollider(AnimatedCollider&&) = delete;

	AnimatedCollider& operator=(AnimatedCollider&&) = delete;

	GO3DAnimated* wrapedGraphics;

	SphereWithVisualization* sphere;

	std::vector<OrientedBoundingBoxWithVisualization*> obbs;



};

#endif // ANIMATEDCOLLIDER_H