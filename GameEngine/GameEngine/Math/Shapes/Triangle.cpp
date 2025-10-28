#include "Triangle.h"

#include "Plane.h"
#include "LineSegment3D.h"
#include "Sphere.h"
#include "../SAT/Interval3D.h"
#include "OrientedBoundingBox.h"
#include "../Math.h"
#include "Ray.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"

#include <glm/gtc/matrix_access.hpp>

#include <cmath>

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

bool Triangle::TriangleIntersect(const Triangle& other, glm::vec3& outHit) const
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

	// === Step 1: Compute Planes ===
	Plane P1 = ConstructPlane();
	Plane P2 = other.ConstructPlane();

	// === Step 2: Handle Parallel / Coplanar Cases ===
	if (std::abs(glm::dot(P1.GetNormal(), P2.GetNormal())) > 1.0 - FLT_EPSILON) {
		// Planes are parallel
		// Check if T2 is on P1
		if (std::abs(P1.SignedDistance(other.point0)) < FLT_EPSILON) {
			// Planes are coplanar. This is a 2D problem.
			// We do not handle this case as requested.
			return false;
		}
		// Planes are parallel and distinct. No intersection.
		return false;
	}

	// === Step 3: Clip T1 against P2 ===
	LineSegment3D S1; // The intersection segment of T1 and P2
	if (!ClipTriangleAgainstPlane(P2, S1)) {
		// T1 is entirely on one side of P2 or intersection is degenerate
		return false;
	}

	// === Step 4: Clip T2 against P1 ===
	LineSegment3D S2; // The intersection segment of T2 and P1
	if (!other.ClipTriangleAgainstPlane(P1, S2)) {
		// T2 is entirely on one side of P1 or intersection is degenerate
		return false;
	}

	// === Step 5: Find 1D Overlap of S1 and S2 ===
	// Both S1 and S2 lie on the same infinite line (the intersection of P1 and P2).
	// We can project them onto that line (represented by S1's direction)
	// and find the 1D interval overlap.

	glm::vec3 L_dir = glm::normalize(S1.GetEnd() - S1.GetStart());
	float L_len_sq = glm::dot(S1.GetStart() - S1.GetEnd(),S1.GetStart() - S1.GetEnd());

	if (L_len_sq < FLT_EPSILON) {
		// S1 is a single point. Degenerate.
		return false;
	}

	// Parameterize the line L as: P(t) = S1[0] + L_dir * t
	// Project S1 onto this line. By definition, its interval is [0, length(S1)].
	float t_S1_start = 0.0;
	float t_S1_end = glm::length(S1.GetEnd() - S1.GetStart());

	// Project S2 onto the same line
	float t_S2_start = glm::dot(S2.GetStart() - S1.GetStart(), L_dir);
	float t_S2_end = glm::dot(S2.GetEnd() - S1.GetStart(), L_dir);

	// Ensure S2 interval is correctly ordered
	if (t_S2_start > t_S2_end) {
		std::swap(t_S2_start, t_S2_end);
	}

	// === Step 6: Find the overlap of [t_S1_start, t_S1_end] and [t_S2_start, t_S2_end] ===
	float t_overlap_start = std::max(t_S1_start, t_S2_start);
	float t_overlap_end = std::min(t_S1_end, t_S2_end);

	// Check if there is a valid overlap
	if (t_overlap_start > t_overlap_end - FLT_EPSILON) {
		// No overlap
		return false;
	}

	// === Step 7: Convert 1D overlap back to 3D points ===
	LineSegment3D intersectionLine(S1.GetStart() + L_dir * t_overlap_start, S1.GetStart() + L_dir * t_overlap_end);

	outHit = (intersectionLine.GetStart() + intersectionLine.GetEnd()) / 2.0f;

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

bool Triangle::ClipTriangleAgainstPlane(const Plane& plane, LineSegment3D& outSegment) const
{
	std::vector<glm::vec3> triangle = { point0, point1, point2 };

	// 1. Get signed distances for all 3 vertices
	float dists[3];
	for (int i = 0; i < 3; ++i) {
		dists[i] = plane.SignedDistance(triangle[i]);
	}

	// 2. Find intersection points
	// We iterate through the triangle's edges (0-1, 1-2, 2-0)
	std::vector<glm::vec3> intersectionPoints;
	for (int i = 0; i < 3; ++i) {
		int j = (i + 1) % 3; // The next vertex in the loop

		const glm::vec3& p1 = triangle[i];
		const glm::vec3& p2 = triangle[j];
		float d1 = dists[i];
		float d2 = dists[j];

		// If signs are different, the edge crosses the plane
		if (d1 * d2 < 0.0) {
			// Find the intersection point using linear interpolation
			float t = d1 / (d1 - d2); // Interpolation factor
			glm::vec3 intersection = p1 + (p2 - p1) * t;
			intersectionPoints.push_back(intersection);
		}
		// Else if a vertex is *on* the plane, add it
		// We check for duplicates, though unlikely with < 0 check above
		else if (std::abs(d1) < FLT_EPSILON) {
			bool found = false;
			for (const auto& p : intersectionPoints) {
				if ((p - p1).length() < FLT_EPSILON) {
					found = true;
					break;
				}
			}
			if (!found) {
				intersectionPoints.push_back(p1);
			}
		}
	}

	// A valid intersection must be a line segment (2 points).
	// 3 points can happen if one vertex is on the plane and two edges cross,
	// but the two edge-crossing points and the vertex will form a single line.
	// For this implementation, we'll assume the robust case finds 2 points.
	if (intersectionPoints.size() == 2) {
		outSegment = LineSegment3D(intersectionPoints[0], intersectionPoints[1]);
		return true;
	}

	// Other cases (e.g., 0, 1, or 3+ points) are degenerate or coplanar.
	return false;
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
