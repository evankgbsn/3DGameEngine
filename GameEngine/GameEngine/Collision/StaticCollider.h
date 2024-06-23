#ifndef STATICCOLLIDER_H
#define STATICCOLLIDER_H

#include "Collider.h"
#include "AxisAlignedBoundingBoxWithVisualization.h"

#include <glm/glm.hpp>

class GO3D;
class AnimatedCollider;
class OrientedBoundingBoxWithVisualization;
class OrientedBoundingBox;
class AxisAlignedBoundingBoxWithVisualization;
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

	float Intersect(const Ray& ray) const;

	const OrientedBoundingBoxWithVisualization* const GetBox() const;

	const SphereWithVisualization* const GetSphere() const;

	const Model* const GetModel() const;

	glm::mat4 GetTransform() const;

	void Translate(const glm::vec3& translation);

private:

	struct BVHNode
	{
		BVHNode() :
			children(nullptr),
			numTriangles(0),
			triangles(nullptr)
		{
		};
		AxisAlignedBoundingBoxWithVisualization bounds;
		BVHNode* children;
		int numTriangles;
		int* triangles;
	};

	StaticCollider(const StaticCollider&) = delete;

	StaticCollider& operator=(const StaticCollider&) = delete;

	StaticCollider(StaticCollider&&) = delete;

	StaticCollider& operator=(StaticCollider&&) = delete;

	void AccelerateMesh();

	void SplitBVHNode(BVHNode* node, int depth);

	void FreeBVHNode(BVHNode* node);

	BVHNode* accelerator;

	GO3D* wrapedGraphics;

	OrientedBoundingBoxWithVisualization* obb;

	SphereWithVisualization* boundingSphere;

	GOColored* trianglesColliderVisualization;



};

#endif // STATICCOLLIDER_H