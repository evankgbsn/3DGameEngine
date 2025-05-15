#ifndef BOUNDINGSPHERECOMPONENT_H
#define BOUNDINGSPHERECOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class SphereWithVisualization;
class GraphicsObject;
class OrientedBoundingBoxComponent;
class AxisAlignedBoundingBoxComponent;
class LineSegment3D;
class Plane;
class Ray;

class BoundingSphereComponent : public Component
{
public:

	BoundingSphereComponent();

	BoundingSphereComponent(float radius);

	BoundingSphereComponent(GraphicsObject* graphics);

	~BoundingSphereComponent();

	void UpdateTransform(const glm::mat4& transform);

	void SetColor(const glm::vec3& color);

	void ToggleVisibility();

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetOffset() const;

	void SetOffset(const glm::vec3& newOffset);

	float GetRadius() const;

	void SetRadius(float newRadius);

	void SetOrigin(const glm::vec3& newOrigin);

	void Transform(const glm::mat4& transform);

	bool PointIntersect(const glm::vec3& point) const;

	bool SphereIntersect(const BoundingSphereComponent& other) const;

	bool AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBoxComponent& aabb) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBoxComponent& obb) const;

	bool PlaneIntersect(const Plane& plane) const;

	float RayIntersect(const Ray& ray) const;

	bool LineSegmentIntersect(const LineSegment3D& line) const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

private:

	BoundingSphereComponent(const BoundingSphereComponent&) = delete;

	BoundingSphereComponent& operator=(const BoundingSphereComponent&) = delete;

	BoundingSphereComponent(BoundingSphereComponent&&) = delete;

	BoundingSphereComponent& operator=(BoundingSphereComponent&&) = delete;

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	SphereWithVisualization* sphere;
};


#endif // BOUNDINGSPHERECOMPONENT_H