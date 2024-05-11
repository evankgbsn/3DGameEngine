#include "Triangle.h"

#include "Plane.h"
#include "LineSegment3D.h"
#include "Sphere.h"

Triangle::~Triangle()
{
}

const glm::vec3& Triangle::GetPoint0() const
{
	return point0;
}

const glm::vec3& Triangle::GetPoint1() const
{
	return point1;
}

const glm::vec3& Triangle::GetPoint2() const
{
	return point2;
}

bool Triangle::PointIntersect(const glm::vec3& point) const
{
	glm::vec3 a = point0 - point;
	glm::vec3 b = point1 - point;
	glm::vec3 c = point2 - point;

	glm::vec3 normPBC = glm::cross(b, c);
	glm::vec3 normPCA = glm::cross(c, a);
	glm::vec3 normPAB = glm::cross(a, b);

	if (glm::dot(normPBC, normPCA) < 0.0f)
	{
		return false;
	}
	else if (glm::dot(normPBC, normPAB) < 0.0f)
	{
		return false;
	}

	return true;
}

bool Triangle::SphereIntersect(const Sphere& sphere) const
{
	glm::vec3 closest = ClosestPoint(sphere.GetOrigin());
	float magSq = glm::dot(closest - sphere.GetOrigin(), closest - sphere.GetOrigin());
	return magSq <= sphere.GetRadius() * sphere.GetRadius();
}

Plane Triangle::ConstructPlane() const
{
	glm::vec3 planeNormal;
	float planeDistance;

	planeNormal = glm::normalize(glm::cross(point1 - point0, point2 - point0));
	planeDistance = glm::dot(planeNormal, point0);

	return Plane(planeNormal, planeDistance);
}

glm::vec3 Triangle::ClosestPoint(const glm::vec3& point) const
{
	Plane plane = ConstructPlane();

	glm::vec3 closest = plane.ClosestPoint(point);

	if (PointIntersect(closest))
	{
		return closest;
	}

	LineSegment3D lc1(point0, point1);
	LineSegment3D lc2(point1, point2);
	LineSegment3D lc3(point2, point0);

	glm::vec3 c1 = lc1.ClosestPoint(point);
	glm::vec3 c2 = lc2.ClosestPoint(point);
	glm::vec3 c3 = lc3.ClosestPoint(point);

	float magSq1 = glm::dot(point - c1, point - c1);
	float magSq2 = glm::dot(point - c2, point - c2);
	float magSq3 = glm::dot(point - c3, point - c3);

	if (magSq1 < magSq2 && magSq1 < magSq3)
	{
		return c1;
	}
	else if (magSq2 < magSq1 && magSq2 < magSq3)
	{
		return c2;
	}

	return c3;
}
