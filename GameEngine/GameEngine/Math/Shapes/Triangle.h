#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>

class Plane;
class Sphere;
class AxisAlignedBoundingBox;
class OrientedBoundingBox;
class Ray;
class LineSegment3D;

class Triangle
{
public:

	Triangle(const glm::vec3& intitialPoint0, const glm::vec3& initialPoint1, const glm::vec3& intitialPoint2);

	~Triangle();

	const glm::vec3& GetPoint0() const;

	const glm::vec3& GetPoint1() const;

	const glm::vec3& GetPoint2() const;

	Plane ConstructPlane() const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

	bool PointIntersect(const glm::vec3& point) const;

	bool SphereIntersect(const Sphere& sphere) const;

	bool AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& aabb) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const;

	bool PlaneIntersect(const Plane& plane) const;

	bool TriangleIntersect(const Triangle& other) const;

	bool TriangleIntersectRobust(const Triangle& other) const;

	bool LineIntersect(const LineSegment3D& line) const;

	glm::vec3 Barycentric(const glm::vec3& p) const;

	float Raycast(const Ray& ray) const;

private:

	bool OverlapOnAxis(const AxisAlignedBoundingBox& aabb, const glm::vec3& axis) const;

	bool OverlapOnAxis(const OrientedBoundingBox& obb, const glm::vec3& axis) const;

	bool OverlapOnAxis(const Triangle& triangle, const glm::vec3& axis) const;

	Triangle(const Triangle&) = delete;

	Triangle& operator=(const Triangle&) = delete;

	Triangle(Triangle&&) = delete;

	Triangle& operator=(Triangle&&) = delete;

	glm::vec3 point0;

	glm::vec3 point1;

	glm::vec3 point2;
};


#endif // TRIANGLE_H