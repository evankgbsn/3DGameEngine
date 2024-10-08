#include "AxisAlignedBoundingBox.h"

#include "Sphere.h"
#include "OrientedBoundingBox.h"
#include "Plane.h"
#include "../SAT/Interval3D.h"
#include "Ray.h"
#include "LineSegment3D.h"

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize) :
	origin(initialOrigin),
	size(initialSize)
{
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox() :
	origin({0.0f, 0.0f, 0.0f}),
	size({ 1.0f, 1.0f, 1.0f })
{
}

AxisAlignedBoundingBox::~AxisAlignedBoundingBox()
{
}

void AxisAlignedBoundingBox::FromMinAndMax(const glm::vec3& min, const glm::vec3& max)
{
	origin = (min + max) * 0.5f;
	size = (max - min) * 0.5f;
}

const glm::vec3& AxisAlignedBoundingBox::GetOrigin() const
{
	return origin;
}

const glm::vec3& AxisAlignedBoundingBox::GetSize() const
{
	return size;
}

glm::vec3 AxisAlignedBoundingBox::GetMin() const
{
	glm::vec3 p1 = origin + size;
	glm::vec3 p2 = origin - size;

	return glm::vec3
	(
		fminf(p1.x, p2.x),
		fminf(p1.y, p2.y),
		fminf(p1.z, p2.z)
	);
}

glm::vec3 AxisAlignedBoundingBox::GetMax() const
{
	glm::vec3 p1 = origin + size;
	glm::vec3 p2 = origin - size;

	return glm::vec3
	(
		fmaxf(p1.x, p2.x),
		fmaxf(p1.y, p2.y),
		fmaxf(p1.z, p2.z)
	);
}

bool AxisAlignedBoundingBox::PointIntersect(const glm::vec3& point) const
{
	const glm::vec3 min = GetMin();
	const glm::vec3 max = GetMax();

	if (min.x > point.x || min.y > point.y || min.z > point.z)
		return false;

	if (max.x < point.x || max.y < point.y || max.z < point.z)
		return false;

	return true;
}

bool AxisAlignedBoundingBox::SphereIntersect(const Sphere& sphere)
{
	return sphere.AxisAlignedBoundingBoxIntersect(*this);
}

bool AxisAlignedBoundingBox::AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& other) const
{
	const glm::vec3 amin = GetMin();
	const glm::vec3 amax = GetMax();

	const glm::vec3 bmin = other.GetMin();
	const glm::vec3 bmax = other.GetMax();

	return (amin.x <= bmax.x && amax.x >= bmin.x) &&
		   (amin.y <= bmax.y && amax.y >= bmin.y) &&
		   (amin.z <= bmax.z && amax.z >= bmin.z);
}

bool AxisAlignedBoundingBox::OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const
{
	const glm::mat4& orientation = obb.GetOrientation();

	glm::vec3 test[15] =
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		orientation[0],
		orientation[1],
		orientation[2]
	};

	for (unsigned int i = 0; i < 3U; ++i)
	{
		test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
		test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
		test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
	}

	for (unsigned int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(obb, test[i]))
			return false;
	}

	return true;
}

bool AxisAlignedBoundingBox::PlaneIntersect(const Plane& plane) const
{
	const glm::vec3& normal = plane.GetNormal();

	float pLen = size.x * fabsf(normal.x) +
				 size.y * fabsf(normal.y) +
				 size.z * fabsf(normal.z);

	float dot = glm::dot(normal, origin);
	float dist = dot - plane.GetDistance();

	return fabsf(dist) <= pLen;
}

float AxisAlignedBoundingBox::RayIntersect(const Ray& ray) const
{
	glm::vec3 min = GetMin();
	glm::vec3 max = GetMax();

	const glm::vec3& rayOrigin = ray.GetOrigin();
	const glm::vec3& rayDirection = ray.GetDirection();

	float t1 = (min.x - rayOrigin.x) / rayDirection.x;
	float t2 = (max.x - rayOrigin.x) / rayDirection.x;
	float t3 = (min.y - rayOrigin.y) / rayDirection.y;
	float t4 = (max.y - rayOrigin.y) / rayDirection.y;
	float t5 = (min.z - rayOrigin.z) / rayDirection.z;
	float t6 = (max.z - rayOrigin.z) / rayDirection.z;

	// Find the largest minimum value.
	float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));

	// Find the smallest maximum value.
	float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

	// AABB is behind the Ray.
	if (tmax < 0.0f)
	{
		return -1.0f;
	}

	// No intersection.
	if (tmin > tmax)
	{
		return -1.0f;
	}

	// Ray origin within the AABB
	if (tmin < 0.0f)
	{
		return tmax;
	}

	return tmin;
}

bool AxisAlignedBoundingBox::LineIntersect(const LineSegment3D& line) const
{
	Ray ray(line.GetStart(), glm::normalize(line.GetEnd() - line.GetStart()));

	float t = RayIntersect(ray);

	return t >= 0 && t * t <= line.GetLengthSq();
}

glm::vec3 AxisAlignedBoundingBox::ClosestPoint(const glm::vec3& point) const
{
	const glm::vec3 min = GetMin();
	const glm::vec3 max = GetMax();

	glm::vec3 closestPoint = point;

	closestPoint.x = (min.x > point.x) ? min.x : point.x;
	closestPoint.y = (min.y > point.y) ? min.y : point.y;
	closestPoint.z = (min.x > point.z) ? min.z : point.z;

	closestPoint.x = (max.x < point.x) ? max.x : point.x;
	closestPoint.y = (max.y < point.y) ? max.y : point.y;
	closestPoint.z = (max.z < point.z) ? max.z : point.z;

	return closestPoint;
}

bool AxisAlignedBoundingBox::OverlapOnAxis(const OrientedBoundingBox& obb, const glm::vec3& axis) const
{
	Interval3D a(*this, axis);
	Interval3D b(obb, axis);
	return ((b.GetMin() <= a.GetMax()) && (a.GetMin() <= b.GetMax()));
}
