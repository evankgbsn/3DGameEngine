#include "OrientedBoundingBoxComponent.h"

#include "../../Collision/OrientedBoundingBoxWithVisualization.h"
#include "../../Math/Shapes/Plane.h"

OrientedBoundingBoxComponent::OrientedBoundingBoxComponent(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation)
{
	RegisterComponentClassType<OrientedBoundingBoxComponent>(this);
	collider = new OrientedBoundingBoxWithVisualization(initialOrigin, initialSize, initialOrientation);
}

OrientedBoundingBoxComponent::OrientedBoundingBoxComponent(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation)
{
	RegisterComponentClassType<OrientedBoundingBoxComponent>(this);
	collider = new OrientedBoundingBoxWithVisualization(vertices, initialOrientation);
}

OrientedBoundingBoxComponent::~OrientedBoundingBoxComponent()
{
	delete collider;
}

void OrientedBoundingBoxComponent::UpdateCollider(const glm::mat4& transform)
{
	collider->Update(transform);
}

void OrientedBoundingBoxComponent::ToggleVisibility()
{
	collider->ToggleVisibility();
}

void OrientedBoundingBoxComponent::SetColor(const glm::vec4& newColor)
{
	collider->SetColor(newColor);
}

const glm::vec4 OrientedBoundingBoxComponent::GetColor() const
{
	return collider->GetColor();
}

const glm::vec3& OrientedBoundingBoxComponent::GetOrigin() const
{
	return collider->GetOrigin();
}

const glm::vec3& OrientedBoundingBoxComponent::GetSize() const
{
	return collider->GetSize();
}

const glm::mat4& OrientedBoundingBoxComponent::GetOrientation() const
{
	return collider->GetOrientation();
}

const glm::vec3& OrientedBoundingBoxComponent::GetOffset() const
{
	return collider->GetOffset();
}

void OrientedBoundingBoxComponent::SetOrigin(const glm::vec3& newOrigin)
{
	collider->SetOrigin(newOrigin);
}

void OrientedBoundingBoxComponent::SetSize(const glm::vec3& newSize)
{
	collider->SetSize(newSize);
}

void OrientedBoundingBoxComponent::SetOrientation(const glm::mat4& newOrientation)
{
	collider->SetOrientation(newOrientation);
}

glm::vec3 OrientedBoundingBoxComponent::ClosestPoint(const glm::vec3& point) const
{
	return collider->ClosestPoint(point);
}

bool OrientedBoundingBoxComponent::PointIntersect(const glm::vec3& point) const
{
	return collider->PointIntersect(point);
}

bool OrientedBoundingBoxComponent::OrientedBoundingBoxIntersect(const OrientedBoundingBoxComponent& other) const
{
	return collider->OrientedBoundingBoxIntersect(*other.GetCollider());
}

bool OrientedBoundingBoxComponent::PlaneIntersect(const Plane& plane) const
{
	return collider->PlaneIntersect(plane);
}

float OrientedBoundingBoxComponent::RayIntersect(const Ray& ray) const
{
	return collider->RayIntersect(ray);
}

bool OrientedBoundingBoxComponent::LineIntersect(const LineSegment3D& line) const
{
	return collider->LineIntersect(line);
}

void OrientedBoundingBoxComponent::SizeToMesh(const std::vector<Vertex>& vertices)
{
	collider->SizeToMesh(vertices);
}

void OrientedBoundingBoxComponent::UpdateOrigin(const glm::mat4& mat)
{
	collider->UpdateOrigin(mat);
}

OrientedBoundingBoxWithVisualization* const OrientedBoundingBoxComponent::GetCollider() const
{
	return collider;
}

void OrientedBoundingBoxComponent::Update()
{
}

void OrientedBoundingBoxComponent::Serialize()
{
	savedBools["IsVisible"] = collider->IsVisible();
	savedVec3s["Origin"] = collider->GetOrigin();
	savedMat4s["Orientation"] = collider->GetOrientation();
	savedVec3s["Color"] = collider->GetColor();
	savedVec3s["Offset"] = collider->GetOffset();
	savedVec3s["Size"] = collider->GetSize();
}

void OrientedBoundingBoxComponent::Deserialize()
{
	collider = new OrientedBoundingBoxWithVisualization(savedVec3s["Origin"], savedVec3s["Size"], savedMat4s["Orientation"]);

	collider->SetColor(glm::vec4(savedVec3s["Color"], 1.0f));

	if (!savedBools["IsVisible"])
	{
		collider->ToggleVisibility();
	}
}
