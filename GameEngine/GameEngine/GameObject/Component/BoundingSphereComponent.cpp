#include "BoundingSphereComponent.h"

#include "../../GameEngine/Collision/SphereWithVisualization.h"
#include "../../GameEngine/GameObject/Component/OrientedBoundingBoxComponent.h"
#include "../../GameEngine/Collision/OrientedBoundingBoxWithVisualization.h"

BoundingSphereComponent::BoundingSphereComponent()
{
	RegisterComponentClassType<BoundingSphereComponent>(this);
}

BoundingSphereComponent::BoundingSphereComponent(float radius) :
	sphere(new SphereWithVisualization(radius))
{
	RegisterComponentClassType<BoundingSphereComponent>(this);
}

BoundingSphereComponent::BoundingSphereComponent(GraphicsObject* graphics) :
	sphere(new SphereWithVisualization(graphics))
{
	RegisterComponentClassType<BoundingSphereComponent>(this);
}

BoundingSphereComponent::~BoundingSphereComponent()
{
	delete sphere;
}

void BoundingSphereComponent::UpdateTransform(const glm::mat4& transform)
{
	sphere->Update(transform);
}

void BoundingSphereComponent::SetColor(const glm::vec3& color)
{
	sphere->SetColor(color);
}

void BoundingSphereComponent::ToggleVisibility()
{
	sphere->ToggleVisibility();
}

bool BoundingSphereComponent::IsVisible() const
{
	return sphere->IsVisible();
}

const glm::vec3& BoundingSphereComponent::GetOrigin() const
{
	return sphere->GetOrigin();
}

const glm::vec3& BoundingSphereComponent::GetOffset() const
{
	return sphere->GetOffset();
}

void BoundingSphereComponent::SetOffset(const glm::vec3& newOffset)
{
	sphere->SetOffset(newOffset);
}

float BoundingSphereComponent::GetRadius() const
{
	return sphere->GetRadius();
}

void BoundingSphereComponent::SetRadius(float newRadius)
{
	sphere->SetRadius(newRadius);
}

void BoundingSphereComponent::SetOrigin(const glm::vec3& newOrigin)
{
	sphere->SetOrigin(newOrigin);
}

void BoundingSphereComponent::Transform(const glm::mat4& transform)
{
	sphere->Transform(transform);
}

bool BoundingSphereComponent::PointIntersect(const glm::vec3& point) const
{
	return sphere->PointIntersect(point);
}

bool BoundingSphereComponent::SphereIntersect(const BoundingSphereComponent& other) const
{
	return sphere->SphereIntersect(*other.sphere);
}

bool BoundingSphereComponent::AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBoxComponent& aabb) const
{
	return false;
}

bool BoundingSphereComponent::OrientedBoundingBoxIntersect(const OrientedBoundingBoxComponent& obb) const
{
	return sphere->OrientedBoundingBoxIntersect(*obb.GetCollider());
}

bool BoundingSphereComponent::PlaneIntersect(const Plane& plane) const
{
	return sphere->PlaneIntersect(plane);
}

float BoundingSphereComponent::RayIntersect(const Ray& ray) const
{
	return sphere->RayIntersect(ray);
}

bool BoundingSphereComponent::LineSegmentIntersect(const LineSegment3D& line) const
{
	return sphere->LineSegmentIntersect(line);
}

glm::vec3 BoundingSphereComponent::ClosestPoint(const glm::vec3& point) const
{
	return sphere->ClosestPoint(point);
}

void BoundingSphereComponent::Serialize()
{
	savedVec3s["Origin"] = GetOrigin();
	savedVec3s["Offset"] = GetOffset();
	savedFloats["Radius"] = GetRadius();
	savedMat4s["Transform"] = sphere->GetTransform();
}

void BoundingSphereComponent::Deserialize()
{
	SetOrigin(savedVec3s["Origin"]);
	SetOffset(savedVec3s["Offset"]);
	SetRadius(savedFloats["Radius"]);
	sphere->Transform(savedMat4s["Transform"]);
}

void BoundingSphereComponent::Update()
{

}
