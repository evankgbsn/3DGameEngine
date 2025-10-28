#include "StaticColliderComponent.h"

#include "../../Collision/StaticCollider.h"
#include "GraphicsObject3DComponent.h"
#include "OrientedBoundingBoxComponent.h"
#include "../../Collision/OrientedBoundingBoxWithVisualization.h"
#include "AnimatedColliderComponent.h"

StaticColliderComponent::StaticColliderComponent() :
	collider(nullptr)
{
	RegisterComponentClassType<StaticColliderComponent>(this);
}

StaticColliderComponent::StaticColliderComponent(GraphicsObject3DComponent* graphics)
{
	RegisterComponentClassType<StaticColliderComponent>(this);
	collider = new StaticCollider(graphics->GetGraphics());
}

StaticColliderComponent::~StaticColliderComponent()
{
	if (collider != nullptr)
	{
		delete collider;
	}
}

void StaticColliderComponent::UpdateCollider()
{
	if (collider != nullptr)
	{
		collider->Update();
	}
}

void StaticColliderComponent::ToggleVisibility()
{
	if (collider != nullptr)
	{
		collider->ToggleVisibility();
	}
}

bool StaticColliderComponent::Intersect(const OrientedBoundingBoxComponent& other) const
{
	if (collider != nullptr)
	{
		return collider->Intersect(*other.GetCollider());
	}
	return false;
}

bool StaticColliderComponent::Intersect(const LineSegment3D& other) const
{
	if (collider != nullptr)
	{
		return collider->Intersect(other);
	}
	return false;
}

bool StaticColliderComponent::Intersect(const AnimatedColliderComponent& other, glm::vec3& outHit) const
{
	if (collider != nullptr)
	{
		return other.Intersect(*this, outHit);
	}
}

bool StaticColliderComponent::SphereIntersect(const AnimatedColliderComponent& other) const
{
	return false;
}

bool StaticColliderComponent::BoxIntersect(const AnimatedColliderComponent& other) const
{
	return false;
}

float StaticColliderComponent::Intersect(const Ray& ray) const
{
	if (collider != nullptr)
	{
		return collider->Intersect(ray);
	}
	return -1.0f;
}

const OrientedBoundingBoxWithVisualization* const StaticColliderComponent::GetBox() const
{
	return collider->GetBox();
}

const SphereWithVisualization* const StaticColliderComponent::GetSphere() const
{
	return collider->GetSphere();
}

const Model* const StaticColliderComponent::GetModel() const
{
	if (collider != nullptr)
	{
		return collider->GetModel();
	}

	return nullptr;
}

glm::mat4 StaticColliderComponent::GetTransform() const
{
	if (collider != nullptr)
	{
		return collider->GetTransform();
	}

	return glm::mat4(0.0f);
}

const Model* const StaticColliderComponent::GetWrapedGraphicsModel() const
{
	if (collider != nullptr)
	{
		return collider->GetWrapedGraphicsModel();
	}

	return nullptr;
}

std::vector<Triangle> StaticColliderComponent::GetTriangles() const
{
	if (collider != nullptr)
	{
		return collider->GetTriangles();
	}

	return std::vector<Triangle>();
}

StaticCollider* const StaticColliderComponent::GetCollider() const
{
	return collider;
}

bool StaticColliderComponent::IsVisible() const
{
	if (collider != nullptr)
	{
		return collider->IsVisible();
	}

	return false;
}

void StaticColliderComponent::SetGraphics(GraphicsObject3DComponent* graphics)
{
	if (collider != nullptr)
	{
		delete collider;
	}

	collider = new StaticCollider(graphics->GetGraphics());
}

glm::vec3 StaticColliderComponent::GetSphereOrigin() const
{
	return collider->GetSphereOrigin();
}

glm::vec3 StaticColliderComponent::GetBoxOrigin() const
{
	return collider->GetBoxOrigin();
}

void StaticColliderComponent::Update()
{
	if (collider != nullptr)
	{
		collider->Update();
	}
}

void StaticColliderComponent::Serialize()
{
}

void StaticColliderComponent::Deserialize()
{
	
}
