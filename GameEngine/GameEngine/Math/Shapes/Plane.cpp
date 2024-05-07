#include "Plane.h"

#include "Ray.h"

Plane::Plane(const glm::vec3& initialNormal, float initialDistanceFromOrigin) :
    normal(initialNormal),
    distance(initialDistanceFromOrigin)
{
}

Plane::~Plane()
{
}

const glm::vec3& Plane::GetNormal() const
{
    return normal;
}

float Plane::GetDistance() const
{
    return distance;
}

float Plane::PlaneEquation(const glm::vec3& point) const
{
    return glm::dot(point, normal) - distance;
}

bool Plane::PointOnPlane(const glm::vec3& point) const
{
    float planeEq = PlaneEquation(point);
    return planeEq >= -0.000005f && planeEq <= 0.000005f;
}

glm::vec3 Plane::ClosestPoint(const glm::vec3& point) const
{
    return point - normal * PlaneEquation(point);
}

bool Plane::PlaneIntersect(const Plane& plane) const
{
    glm::vec3 zeroVec = glm::cross(normal, plane.normal);
    return glm::dot(zeroVec, zeroVec) != 0.0f;
}

float Plane::RayIntersect(const Ray& ray) const
{
    float nd = glm::dot(ray.GetDirection(), normal);
    float pn = glm::dot(ray.GetOrigin(), normal);

    if (nd >= 0.0f)
    {
        return -1.0f;
    }

    float t = (distance - pn) / nd;

    if (t >= 0.0f)
    {
        return t;
    }

    return -1.0f;
}
