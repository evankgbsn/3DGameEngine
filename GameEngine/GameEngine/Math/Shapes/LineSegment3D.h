#ifndef LINESEGMENT3D_H
#define LINESEGMENT3D_H

#include <glm/glm.hpp>

class LineSegment3D
{
public:

	LineSegment3D(const glm::vec3& startPoint = glm::vec3(0.0f), const glm::vec3& endPoint = glm::vec3(0.0f));

	~LineSegment3D();

	float GetLength() const;

	float GetLengthSq() const;

	const glm::vec3& GetStart() const;

	const glm::vec3& GetEnd() const;

	bool PointIntersect(const glm::vec3& point) const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

	LineSegment3D(const LineSegment3D&) = default;

	LineSegment3D& operator=(const LineSegment3D&) = default;

	LineSegment3D(LineSegment3D&&) = default;

	LineSegment3D& operator=(LineSegment3D&&) = default;

private:

	glm::vec3 start;

	glm::vec3 end;
};

#endif // LINESEGMENT3D_H