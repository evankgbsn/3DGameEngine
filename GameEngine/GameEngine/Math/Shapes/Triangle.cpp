#include "Triangle.h"

#include "Plane.h"
#include "LineSegment3D.h"
#include "Sphere.h"
#include "../SAT/Interval3D.h"
#include "OrientedBoundingBox.h"
#include "../Math.h"
#include "Ray.h"

#include <glm/gtc/matrix_access.hpp>

Triangle::Triangle(const glm::vec3& intitialPoint0, const glm::vec3& initialPoint1, const glm::vec3& intitialPoint2) :
	point0(intitialPoint0),
	point1(initialPoint1),
	point2(intitialPoint2)
{
}

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

bool Triangle::TriangleIntersect(const Triangle& other) const
{
	const glm::vec3 t1_f0 = point1 - point0;
	const glm::vec3 t1_f1 = point2 - point1;
	const glm::vec3 t1_f2 = point0 - point2;
	
	const glm::vec3 t2_f0 = other.point1 - other.point0;
	const glm::vec3 t2_f1 = other.point2 - other.point1;
	const glm::vec3 t2_f2 = other.point0 - other.point2;
	
	glm::vec3 axisToTest[11] =
	{
		glm::cross(t1_f0, t1_f1),
		glm::cross(t2_f0, t2_f1),
		glm::cross(t2_f0, t1_f0), glm::cross(t2_f0, t1_f1),
		glm::cross(t2_f0, t1_f2), glm::cross(t2_f1, t1_f0),
		glm::cross(t2_f1, t1_f1), glm::cross(t2_f1, t1_f2),
		glm::cross(t2_f2, t1_f0), glm::cross(t2_f2, t1_f1),
		glm::cross(t2_f2, t1_f2)
	};
	
	for (unsigned int i = 0; i < 11; ++i)
	{
		if (!OverlapOnAxis(other, axisToTest[i]))
		{
			return false;
		}
	}

	return true;
}

bool Triangle::TriangleIntersectRobust(const Triangle& other) const
{
	glm::vec3 axisToTest[11] =
	{
		Math::SatCrossEdge(point0, point1, point1, point2),
		Math::SatCrossEdge(other.point0, other.point1, other.point1, other.point2),

		Math::SatCrossEdge(other.point0, other.point1, point0, point1),
		Math::SatCrossEdge(other.point0, other.point1, point1, point2),
		Math::SatCrossEdge(other.point0, other.point1, point2, point0),

		Math::SatCrossEdge(other.point1, other.point2, point0, point1),
		Math::SatCrossEdge(other.point1, other.point2, point1, point2),
		Math::SatCrossEdge(other.point1, other.point2, point2, point0),

		Math::SatCrossEdge(other.point2, other.point0, point0, point1),
		Math::SatCrossEdge(other.point2, other.point0, point1, point2),
		Math::SatCrossEdge(other.point2, other.point0, point2, point0)
	};

	for (unsigned int i = 0; i < 11; ++i)
	{
		if (!OverlapOnAxis(other, axisToTest[i]))
		{
			return false;
		}
	}

	return true;
}

bool Triangle::LineIntersect(const LineSegment3D& line) const
{
	Ray ray(line.GetStart(), glm::normalize(line.GetEnd() - line.GetStart()));

	float t = Raycast(ray);

	return t >= 0.0f && t * t <= line.GetLengthSq();
}

glm::vec3 Triangle::Barycentric(const glm::vec3& p) const
{
	const glm::vec3 ap = p - point0;
	const glm::vec3 bp = p - point1;
	const glm::vec3 cp = p - point2;

	const glm::vec3 ab = point1 - point0;
	const glm::vec3 ac = point2 - point0;
	const glm::vec3 bc = point2 - point1;
	const glm::vec3 cb = point1 - point2;
	const glm::vec3 ca = point0 - point2;

	auto proj = [](const glm::vec3& vec, const glm::vec3& w) -> glm::vec3
		{
			return ((glm::dot(vec, w) / glm::dot(w, w)) * w);
		};

	glm::vec3 v = ab - proj(ab, cb);
	float a = 1.0f - (glm::dot(v, ap) / glm::dot(v, ab));

	v = bc - proj(bc, ac);
	float b = 1.0f - (glm::dot(v, bp) / glm::dot(v, bc));

	v = ca - proj(ca, ab);
	float c = 1.0f - (glm::dot(v, cp) / glm::dot(v, ca));

	return glm::vec3(a, b, c);
}

float Triangle::Raycast(const Ray& ray) const
{
	Plane plane = ConstructPlane();

	float t = plane.RayIntersect(ray);
	if (t < 0.0f)
	{
		return t;
	}

	const glm::vec3 result = ray.GetOrigin() + ray.GetDirection() * t;

	glm::vec3 barycentric = Barycentric(result);
	if (barycentric.x >= 0.0f && barycentric.x <= 1.0f &&
		barycentric.y >= 0.0f && barycentric.y <= 1.0f &&
		barycentric.z >= 0.0f && barycentric.z <= 1.0f)
	{
		return t;
	}
	
	return -1;
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

bool Triangle::OverlapOnAxis(const Triangle& triangle, const glm::vec3& axis) const
{
	Interval3D a(triangle, axis);
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
