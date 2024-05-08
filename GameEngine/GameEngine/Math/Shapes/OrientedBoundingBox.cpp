#include "OrientedBoundingBox.h"

#include "../SAT/Interval3D.h"
#include "../../Renderer/Model/Vertex.h"
#include "Plane.h"
#include "Ray.h"
#include "LineSegment3D.h"

#include <glm/gtc/matrix_access.hpp>

OrientedBoundingBox::OrientedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation) :
    origin(initialOrigin),
    size(initialSize),
    orientation(initialOrientation)
{
}

OrientedBoundingBox::OrientedBoundingBox(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation) :
    orientation(initialOrientation)
{
    SizeToMesh(vertices);
}

OrientedBoundingBox::~OrientedBoundingBox()
{
}

const glm::vec3& OrientedBoundingBox::GetOrigin() const
{
    return origin;
}

const glm::vec3& OrientedBoundingBox::GetSize() const
{
    return size;
}

const glm::mat4& OrientedBoundingBox::GetOrientation() const
{
    return orientation;
}

const glm::vec3& OrientedBoundingBox::GetOffset() const
{
    return offset;
}

void OrientedBoundingBox::SetOrigin(const glm::vec3& newOrigin)
{
    origin = newOrigin;
}

bool OrientedBoundingBox::PointIntersect(const glm::vec3& point) const
{
    glm::vec3 dir = point - origin;

    for (unsigned int i = 0; i < 3; ++i)
    {
        glm::vec3 axis = glm::normalize(orientation[i]);

        float distance = glm::dot(dir, axis);

        if (distance > size[i])
            return false;

        if (distance < -size[i])
            return false;
    }

    return true;
}

bool OrientedBoundingBox::OrientedBoundingBoxIntersect(const OrientedBoundingBox& other) const
{
    const glm::mat4& otherRot = other.GetOrientation();
    const glm::mat4& rot = orientation;

    glm::vec3 test[15] =
    {
        otherRot[0],
        otherRot[1],
        otherRot[2],
        rot[0],
        rot[1],
        rot[2]
    };

    for (unsigned int i = 0; i < 3U; ++i)
    {
        test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
        test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
        test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
    }

    for (unsigned int i = 0; i < 15; ++i)
    {
        if (!OverlapOnAxis(other, test[i]))
            return false;
    }

    return true;
}

bool OrientedBoundingBox::PlaneIntersect(const Plane& plane) const
{
    const glm::mat4& rot = orientation;
    const glm::vec3& normal = plane.GetNormal();


    float pLen = size.x * fabsf(glm::dot(normal, glm::vec3(rot[0]))) +
                 size.y * fabsf(glm::dot(normal, glm::vec3(rot[1]))) +
                 size.z * fabsf(glm::dot(normal, glm::vec3(rot[2])));

    float dot = glm::dot(normal, origin);
    float dist = dot - plane.GetDistance();

    return fabsf(dist) <= pLen;
}

float OrientedBoundingBox::RayIntersect(const Ray& ray) const
{
    const glm::vec3& s = GetSize();
    const glm::mat4& o = GetOrientation();

    glm::vec3 p = origin - ray.GetOrigin();
    const glm::vec3& rayDirection = ray.GetDirection();

    glm::vec3 f = glm::vec3( glm::dot(glm::vec3(glm::column(o, 0)), rayDirection), 
                             glm::dot(glm::vec3(glm::column(o, 1)), rayDirection),
                             glm::dot(glm::vec3(glm::column(o, 2)), rayDirection));

    glm::vec3 e = glm::vec3(glm::dot(glm::vec3(glm::column(o, 0)), p),
                            glm::dot(glm::vec3(glm::column(o, 1)), p),
                            glm::dot(glm::vec3(glm::column(o, 2)), p));

    float t[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    for (unsigned int i = 0; i < 3; ++i)
    {
        if (abs(f[i]) < 0.000001f)
        {
            if (-e[i] - s[i] > 0.0f || -e[i] + s[i] < 0.0f)
            {
                return -1;
            }

            f[i] = 0.000001f;
        }

        t[i * 2 + 0] = (e[i] + s[i]) / f[i]; // min
        t[i * 2 + 1] = (e[i] - s[i]) / f[i]; // max
    }

    // Find the largest minimum value.
    float tmin = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));

    // Find the smallest maximum value.
    float tmax = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

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

bool OrientedBoundingBox::LineIntersect(const LineSegment3D& line) const
{
    Ray ray(line.GetStart(), glm::normalize(line.GetEnd() - line.GetStart()));
    float t = RayIntersect(ray);

    return t >= 0 && t * t <= line.GetLengthSq();
}

void OrientedBoundingBox::SizeToMesh(const std::vector<Vertex>& verts)
{
    glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    glm::vec3 point;

    for (const Vertex& x : verts)
    {
         point = x.GetPosition();

        if (point.x >= max.x) max.x = point.x;
        if (point.x <= min.x) min.x = point.x;

        if (point.y >= max.y) max.y = point.y;
        if (point.y <= min.y) min.y = point.y;

        if (point.z >= max.z) max.z = point.z;
        if (point.z <= min.z) min.z = point.z;
    }

    // Eight vertices of the bounding box.
    glm::vec3 vertex[8] =
    {
        glm::vec3(min.x, max.y, max.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        min,
        max,
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, min.y, min.z)
    };
    
    // Half-length of sides
    float halfWidth = glm::length(min - vertex[7]) / 2.0f;
    float halfDepth = glm::length(min - vertex[2]) / 2.0f;
    float halfHeight = glm::length(min - vertex[1]) / 2.0f;

    size = glm::vec3(halfWidth, halfHeight, halfDepth);

    offset = min + ((max - min) / 2.f);
}

void OrientedBoundingBox::UpdateOrigin(const glm::mat4& mat)
{
    origin = mat * glm::vec4(origin, 1.0f);
}

void OrientedBoundingBox::SetSize(const glm::vec3& newSize)
{
    size = newSize;
}

void OrientedBoundingBox::SetOrientation(const glm::mat4& newOrientation)
{
    orientation = newOrientation;
}

glm::vec3 OrientedBoundingBox::ClosestPoint(const glm::vec3& point) const
{
    glm::vec3 result = origin;
    
    glm::vec3 dir = point - origin;

    for (unsigned int i = 0; i < 3; ++i)
    {
        glm::vec3 axis = glm::normalize(orientation[i]);

        float distance = glm::dot(dir, axis);

        if (distance > size[i])
            distance = size[i];

        if (distance < -size[i])
            distance = -size[i];

        result = result + (axis * distance);
    }

    return result;
}

bool OrientedBoundingBox::OverlapOnAxis(const OrientedBoundingBox& other, glm::vec3& axis) const
{
    Interval3D a(*this, axis);
    Interval3D b(other, axis);
    return ((b.GetMin() <= a.GetMax()) && (a.GetMin() <= b.GetMax()));
}
