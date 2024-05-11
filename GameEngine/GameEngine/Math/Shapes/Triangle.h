#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>

class Plane;
class Sphere;

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

private:

	Triangle(const Triangle&) = delete;

	Triangle& operator=(const Triangle&) = delete;

	Triangle(Triangle&&) = delete;

	Triangle& operator=(Triangle&&) = delete;

	glm::vec3 point0;

	glm::vec3 point1;

	glm::vec3 point2;
};


#endif // TRIANGLE_H