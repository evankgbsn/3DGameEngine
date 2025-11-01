#ifndef STATICCOLLIDER_H
#define STATICCOLLIDER_H

#include "Collider.h"
#include "AxisAlignedBoundingBoxWithVisualization.h"

#include <glm/glm.hpp>

#include <vector>

class GO3D;
class AnimatedCollider;
class OrientedBoundingBoxWithVisualization;
class OrientedBoundingBox;
class AxisAlignedBoundingBoxWithVisualization;
class SphereWithVisualization;
class LineSegment3D;
class GOColored;
class Model;
class Triangle;
class GOTexturedLitInstanced;

class StaticCollider : public Collider
{
public:

	StaticCollider(GO3D* const graphicsObject);

	StaticCollider(GOTexturedLitInstanced* const graphicsObjectInstanced, unsigned int instanceID);

	~StaticCollider();

	void Update() override;

	void ToggleVisibility() override;

	bool Intersect(const OrientedBoundingBox& other) const;

	bool Intersect(const LineSegment3D& other) const;

	bool Intersect(const AnimatedCollider& other, glm::vec3& outHit) const;

	float Intersect(const Ray& ray) const;

	const OrientedBoundingBoxWithVisualization* const GetBox() const;

	const SphereWithVisualization* const GetSphere() const;

	const Model* const GetModel() const;

	glm::mat4 GetTransform() const;

	const Model* const GetWrapedGraphicsModel() const;

	std::vector<Triangle> GetTriangles() const;

	glm::vec3 GetSphereOrigin() const;

	glm::vec3 GetBoxOrigin() const;

private:

	friend class AnimatedCollider;

	StaticCollider(const StaticCollider&) = delete;

	StaticCollider& operator=(const StaticCollider&) = delete;

	StaticCollider(StaticCollider&&) = delete;

	StaticCollider& operator=(StaticCollider&&) = delete;

	GO3D* wrapedGraphics;

	GOTexturedLitInstanced* wrapedGraphicsInstanced;

	unsigned int instanceID;

	OrientedBoundingBoxWithVisualization* obb;

	SphereWithVisualization* boundingSphere;

	GOColored* trianglesColliderVisualization;

	glm::vec3 lastTranslation;

};

#endif // STATICCOLLIDER_H