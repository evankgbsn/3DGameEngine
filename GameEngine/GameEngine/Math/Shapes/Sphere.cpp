#include "Sphere.h"

#include "AxisAlignedBoundingBox.h"
#include "OrientedBoundingBox.h"
#include "Plane.h"
#include "Ray.h"
#include "LineSegment3D.h"
#include "../../Renderer/Model/Vertex.h"

Sphere::Sphere(const glm::vec3& initialOrigin, float initialRadius) :
	origin(initialOrigin),
	radius(initialRadius)
{
}

Sphere::Sphere(const std::vector<Vertex>& verts)
{
	// Find min and max on all axes.
	glm::vec3 first = verts[0].GetPosition();
	glm::vec3 minX = first;
	glm::vec3 minY = first;
	glm::vec3 minZ = first;
	glm::vec3 maxX = first;
	glm::vec3 maxY = first;
	glm::vec3 maxZ = first;

	for (const Vertex& vert : verts)
	{
		const glm::vec3& pos = vert.GetPosition();
		if (pos.x <= minX.x) minX = pos;
		if (pos.x >= maxX.x) maxX = pos;

		if (pos.y <= minY.y) minY = pos;
		if (pos.y >= maxY.y) maxY = pos;

		if (pos.z <= minZ.z) minZ = pos;
		if (pos.z >= maxZ.z) maxZ = pos;
	}

	// Determine which axis has greatest range.
	int axis = 0; // x = 0, y = 1, z = 2
	float max = 0, cur = 0;

	if ((cur = glm::length((minX - maxX))) >= max) { axis = 0; max = cur; }
	if ((cur = glm::length((minY - maxY))) >= max) { axis = 1; max = cur; }
	if ((cur = glm::length((minZ - maxZ))) >= max) { axis = 2; max = cur; }

	// Determine center and radius.
	this->radius = max / 2;
	switch (axis)
	{
	case 0:
		origin = minX + glm::normalize((maxX - minX)) * radius;
		break;
	case 1:
		origin = minY + glm::normalize((maxY - minY)) * radius;
		break;
	case 2:
		origin = minZ + glm::normalize((maxZ - minZ)) * radius;
		break;
	}

	// Adjust center and radius.
	for (const Vertex& vert : verts)
	{
		const glm::vec3& pos = vert.GetPosition();
		float dist;
		glm::vec3 direction;
		if ((dist = glm::length(direction = pos - origin)) > radius)
		{
			radius += (dist -= radius);
			origin += glm::normalize(pos) * dist;
		}
	}
	offset = origin;
}

Sphere::~Sphere()
{
}

const glm::vec3& Sphere::GetOrigin() const
{
	return origin;
}

const glm::vec3& Sphere::GetOffset() const
{
	return offset;
}

void Sphere::SetOffset(const glm::vec3& newOffset)
{
	offset = newOffset;
}

float Sphere::GetRadius() const
{
	return radius;
}

void Sphere::SetRadius(float newRadius)
{
	radius = newRadius;
}

void Sphere::SetOrigin(const glm::vec3& newOrigin)
{
	origin = newOrigin;
}

void Sphere::Transform(const glm::mat4& trans)
{

	transform = trans;

	glm::mat4 rotation(1.0f);
	rotation[0] = glm::normalize(transform[0]);
	rotation[1] = glm::normalize(transform[1]);
	rotation[2] = glm::normalize(transform[2]);

	origin = rotation * glm::vec4(offset, 1.0f);
	origin = origin + glm::vec3(transform[3]);
}

bool Sphere::PointIntersect(const glm::vec3& point) const
{
	glm::vec3 toPoint = point - origin;
	return glm::dot(toPoint, toPoint) <= radius * radius;
}

bool Sphere::SphereIntersect(const Sphere& other) const
{
	glm::vec3 distance = other.origin - origin;

	float lengthSq = glm::dot(distance, distance);

	float sumRadii = other.radius + radius;

	return lengthSq <= sumRadii * sumRadii;
}

bool Sphere::AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& aabb) const
{
	glm::vec3 closest = aabb.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

bool Sphere::OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const
{
	glm::vec3 closest = obb.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

bool Sphere::PlaneIntersect(const Plane& plane) const
{
	glm::vec3 closest = plane.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

float Sphere::RayIntersect(const Ray& ray) const
{
	glm::vec3 e = origin - ray.GetOrigin();

	float rSq = radius * radius;

	float eSq = glm::dot(e, e);

	float a = glm::dot(e, ray.GetDirection());

	float bSq = eSq = -(a * a);

	float f = sqrt(rSq - bSq);

	if (rSq - (eSq - (a * a)) < 0.0f)
	{
		return -1.0f;
	}
	else if (eSq < rSq)
	{
		return a + f;
	}

	return a - f;
}

bool Sphere::LineSegmentIntersect(const LineSegment3D& line) const
{
	glm::vec3 closest = line.ClosestPoint(origin);

	float distanceSq = glm::dot(origin - closest, origin - closest);

	return distanceSq <= radius * radius;
}

glm::vec3 Sphere::ClosestPoint(const glm::vec3& point) const
{
	glm::vec3 toPoint = glm::normalize(point - origin);
	return origin + toPoint * radius;
}

glm::mat4 Sphere::GetTransform() const
{
	return transform;
}
