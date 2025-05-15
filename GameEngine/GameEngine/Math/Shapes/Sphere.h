#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>

#include <vector>

class AxisAlignedBoundingBox;
class OrientedBoundingBox;
class Plane;
class Vertex;
class Ray;
class LineSegment3D;

class Sphere
{

public:

	Sphere(const glm::vec3& initialOrigin, float initialRadius);

	Sphere(const std::vector<Vertex>& vertices);

	~Sphere();

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetOffset() const;

	void SetOffset(const glm::vec3& newOffset);

	float GetRadius() const;

	void SetRadius(float newRadius);

	void SetOrigin(const glm::vec3& newOrigin);

	void Transform(const glm::mat4& transform);

	bool PointIntersect(const glm::vec3& point) const;

	bool SphereIntersect(const Sphere& other) const;

	bool AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& aabb) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const;

	bool PlaneIntersect(const Plane& plane) const;

	float RayIntersect(const Ray& ray) const;

	bool LineSegmentIntersect(const LineSegment3D& line) const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

	glm::mat4 GetTransform() const;

private:

	Sphere(const Sphere&) = delete;

	Sphere& operator=(const Sphere&) = delete;
	
	Sphere(Sphere&&) = delete;

	Sphere& operator=(Sphere&&) = delete;

	glm::vec3 origin;

	float radius;

	glm::vec3 offset;

	glm::mat4 transform;
};

#endif // SPHERE_H