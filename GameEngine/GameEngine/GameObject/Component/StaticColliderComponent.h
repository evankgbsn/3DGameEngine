#ifndef STATICCOLLIDERCOMPONENT_H
#define STATICCOLLIDERCOMPONENT_H

#include "Component.h"
#include "../../Math/Shapes/Triangle.h"

#include <glm/glm.hpp>

class StaticCollider;
class GraphicsObject3DComponent;
class OrientedBoundingBoxComponent;
class AnimatedColliderComponent;
class LineSegment3D;
class Ray;
class OrientedBoundingBoxWithVisualization;
class SphereWithVisualization;
class Model;

class StaticColliderComponent : public Component
{
public:

	StaticColliderComponent(GraphicsObject3DComponent* graphics);

	~StaticColliderComponent();

	void UpdateCollider();

	void ToggleVisibility();

	bool Intersect(const OrientedBoundingBoxComponent& other) const;

	bool Intersect(const LineSegment3D& other) const;

	bool Intersect(const AnimatedColliderComponent& other) const;

	float Intersect(const Ray& ray) const;

	const OrientedBoundingBoxWithVisualization* const GetBox() const;

	const SphereWithVisualization* const GetSphere() const;

	const Model* const GetModel() const;

	glm::mat4 GetTransform() const;

	void Translate(const glm::vec3& translation);

	const Model* const GetWrapedGraphicsModel() const;

	std::vector<Triangle> GetTriangles() const;

	StaticCollider* const GetCollider() const;

	bool IsVisible() const;

private:

	StaticColliderComponent() = delete;

	StaticColliderComponent(const StaticColliderComponent&) = delete;

	StaticColliderComponent& operator=(const StaticColliderComponent&) = delete;

	StaticColliderComponent(StaticColliderComponent&&) = delete;

	StaticColliderComponent& operator=(StaticColliderComponent&&) = delete;

	void Update() override;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	StaticCollider* collider;

};


#endif // STATICCOLLIDERCOMPONENT_H