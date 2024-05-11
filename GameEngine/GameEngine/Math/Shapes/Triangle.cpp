#include "Triangle.h"

#include "Plane.h"
#include "LineSegment3D.h"
#include "Sphere.h"
#include "../SAT/Interval3D.h"
#include "OrientedBoundingBox.h"

#include <glm/gtc/matrix_access.hpp>

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

bool Triangle::AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& aabb) const
{
	glm::vec3 f0 = point1 - point0;
	glm::vec3 f1 = point2 - point1;
	glm::vec3 f2 = point0 - point2;

	glm::vec3 u0(1.0f, 0.0f, 0.0f);
	glm::vec3 u1(0.0f, 1.0f, 0.0f);
	glm::vec3 u2(0.0f, 0.0f, 1.0f);

	glm::vec3 test[13] = {
		u0, u1, u2, 
		glm::cross(f0, f1), 
		glm::cross(u0, f0), glm::cross(u0, f1), glm::cross(u0, f2),
		glm::cross(u1, f0), glm::cross(u1, f1), glm::cross(u1, f2),
		glm::cross(u2, f0), glm::cross(u2, f1), glm::cross(u2, f2)
	};

	for (unsigned int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(aabb, test[i]))
		{
			return false;
		}
	}

	return true;
}

bool Triangle::OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const
{
	glm::vec3 f0 = point1 - point0;
	glm::vec3 f1 = point2 - point1;
	glm::vec3 f2 = point0 - point2;

	glm::vec3 u0(glm::column(obb.GetOrientation(), 0));
	glm::vec3 u1(glm::column(obb.GetOrientation(), 1));
	glm::vec3 u2(glm::column(obb.GetOrientation(), 2));

	glm::vec3 test[13] = {
		u0, u1, u2,
		glm::cross(f0, f1),
		glm::cross(u0, f0), glm::cross(u0, f1), glm::cross(u0, f2),
		glm::cross(u1, f0), glm::cross(u1, f1), glm::cross(u1, f2),
		glm::cross(u2, f0), glm::cross(u2, f1), glm::cross(u2, f2)
	};

	for (unsigned int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(obb, test[i]))
		{
			return false;
		}
	}

	return true;
}

bool Triangle::PlaneIntersect(const Plane& plane) const
{
	float side1 = plane.PlaneEquation(point0);
	float side2 = plane.PlaneEquation(point1);
	float side3 = plane.PlaneEquation(point2);

	if (abs(side1) <= 0.00001f && abs(side2) <= 0.00001f && abs(side3) <= 0.00001f)
	{
		return true;
	}

	if (side1 > 0.00001f && side2 > 0.00001f && side3 > 0.00001f)
	{
		return false;
	}

	if (side1 < 0.0f && side2 < 0.0f && side3 < 0.0f)
	{
		return false;
	}

	return true;
}

bool Triangle::OverlapOnAxis(const AxisAlignedBoundingBox& aabb, const glm::vec3& axis) const
{
	Interval3D a(aabb, axis);
	Interval3D b(*this, axis);
	return ((b.GetMin() <= a.GetMax()) && a.GetMin() <= b.GetMax());
}

bool Triangle::OverlapOnAxis(const OrientedBoundingBox& obb, const glm::vec3& axis) const
{
	Interval3D a(obb, axis);
	Interval3D b(*this, axis);
	return ((b.GetMin() <= a.GetMax()) && a.GetMin() <= b.GetMax());
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
