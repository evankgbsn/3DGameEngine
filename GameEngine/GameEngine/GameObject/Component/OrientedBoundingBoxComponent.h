#ifndef ORIENTEDBOUNDINGBOXCOMPONENT_H
#define ORIENTEDBOUNDINGBOXCOMPONENT_H

#include "Component.h"
#include "../../Renderer/Model/Vertex.h"

#include <glm/glm.hpp>

class OrientedBoundingBoxWithVisualization;
class Plane;
class Ray;
class LineSegment3D;

class OrientedBoundingBoxComponent : public Component
{
public:

	OrientedBoundingBoxComponent(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation = glm::mat4(1.0f));

	OrientedBoundingBoxComponent(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation = glm::mat4(1.0f));

	~OrientedBoundingBoxComponent();

	void UpdateCollider(const glm::mat4& transform);

	void ToggleVisibility();

	void SetColor(const glm::vec4& newColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	const glm::vec4 GetColor() const;

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetSize() const;

	const glm::mat4& GetOrientation() const;

	const glm::vec3& GetOffset() const;

	void SetOrigin(const glm::vec3& newOrigin);

	void SetSize(const glm::vec3& newSize);

	void SetOrientation(const glm::mat4& newOrientation);

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

	bool PointIntersect(const glm::vec3& point) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBoxComponent& other) const;

	bool PlaneIntersect(const Plane& plane) const;

	float RayIntersect(const Ray& ray) const;

	bool LineIntersect(const LineSegment3D& line) const;

	void SizeToMesh(const std::vector<Vertex>& vertices);

	void UpdateOrigin(const glm::mat4& mat);

	OrientedBoundingBoxWithVisualization* const GetCollider() const;

private:

	OrientedBoundingBoxComponent() = delete;

	OrientedBoundingBoxComponent(const OrientedBoundingBoxComponent&) = delete;

	OrientedBoundingBoxComponent& operator=(const OrientedBoundingBoxComponent&) = delete;

	OrientedBoundingBoxComponent(OrientedBoundingBoxComponent&&) = delete;

	OrientedBoundingBoxComponent& operator=(OrientedBoundingBoxComponent&&) = delete;

	void Update();

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;


	OrientedBoundingBoxWithVisualization* collider;
};

#endif // ORIENTEDBOUNDINGBOXCOMPONENT_H