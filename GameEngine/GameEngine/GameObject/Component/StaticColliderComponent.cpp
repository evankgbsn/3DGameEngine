#include "StaticColliderComponent.h"

#include "../../Collision/StaticCollider.h"
#include "GraphicsObject3DComponent.h"
#include "OrientedBoundingBoxComponent.h"
#include "../../Collision/OrientedBoundingBoxWithVisualization.h"

StaticColliderComponent::StaticColliderComponent(GraphicsObject3DComponent* graphics)
{
	RegisterComponentClassType<StaticColliderComponent>(this);
	collider = new StaticCollider(graphics->GetGraphics());
}

StaticColliderComponent::~StaticColliderComponent()
{
	delete collider;
}

void StaticColliderComponent::UpdateCollider()
{
	collider->Update();
}

void StaticColliderComponent::ToggleVisibility()
{
	collider->ToggleVisibility();
}

bool StaticColliderComponent::Intersect(const OrientedBoundingBoxComponent& other) const
{
	return collider->Intersect(*other.GetCollider());
}

bool StaticColliderComponent::Intersect(const LineSegment3D& other) const
{
	return collider->Intersect(other);
}

bool StaticColliderComponent::Intersect(const AnimatedColliderComponent& other) const
{
	return false;
}

float StaticColliderComponent::Intersect(const Ray& ray) const
{
	return collider->Intersect(ray);
}

const OrientedBoundingBoxWithVisualization* const StaticColliderComponent::GetBox() const
{
	return nullptr;
}

const SphereWithVisualization* const StaticColliderComponent::GetSphere() const
{
	return nullptr;
}

const Model* const StaticColliderComponent::GetModel() const
{
	return collider->GetModel();
}

glm::mat4 StaticColliderComponent::GetTransform() const
{
	return collider->GetTransform();
}

void StaticColliderComponent::Translate(const glm::vec3& translation)
{
	collider->Translate(translation);
}

const Model* const StaticColliderComponent::GetWrapedGraphicsModel() const
{
	return collider->GetWrapedGraphicsModel();
}

std::vector<Triangle> StaticColliderComponent::GetTriangles() const
{
	return collider->GetTriangles();
}

StaticCollider* const StaticColliderComponent::GetCollider() const
{
	return collider;
}

bool StaticColliderComponent::IsVisible() const
{
	return collider->IsVisible();
}

void StaticColliderComponent::Update()
{
}

const std::vector<char> StaticColliderComponent::Serialize() const
{
	return std::vector<char>();
}

void StaticColliderComponent::Deserialize(const std::vector<char>& data)
{
}
