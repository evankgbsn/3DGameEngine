#ifndef STATICCOLLIDER_H
#define STATICCOLLIDER_H

#include "Collider.h"

#include <glm/glm.hpp>

class GO3D;
class AnimatedCollider;
class OrientedBoundingBoxWithVisualization;
class OrientedBoundingBox;
class AxisAllignedBoundingBox;
class SphereWithVisualization;
class LineSegment3D;
class GOColored;
class Model;

class StaticCollider : public Collider
{
public:

	StaticCollider(GO3D* const graphicsObject);

	~StaticCollider();

	void Update() override;

	void ToggleVisibility() override;

	bool Intersect(const OrientedBoundingBox& other) const;

	bool Intersect(const LineSegment3D& other) const;

	bool Intersect(const AnimatedCollider& other) const;

	const OrientedBoundingBoxWithVisualization* const GetBox() const;

	const SphereWithVisualization* const GetSphere() const;

	const Model* const GetModel() const;

	glm::mat4 GetTransform() const;

private:

	StaticCollider(const StaticCollider&) = delete;

	StaticCollider& operator=(const StaticCollider&) = delete;

	StaticCollider(StaticCollider&&) = delete;

	StaticCollider& operator=(StaticCollider&&) = delete;

	GO3D* wrapedGraphics;

	OrientedBoundingBoxWithVisualization* obb;

	SphereWithVisualization* boundingSphere;

	GOColored* trianglesColliderVisualization;



};

#endif // STATICCOLLIDER_H