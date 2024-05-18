#include "StaticCollider.h"

#include "../Collision/OrientedBoundingBoxWithVisualization.h"
#include "../Collision/AnimatedCollider.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Collision/SphereWithVisualization.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"

StaticCollider::StaticCollider(GO3D* const graphicsObject) :
	wrapedGraphics(graphicsObject)
{
	this->boundingSphere = new SphereWithVisualization(wrapedGraphics);
	this->obb = new OrientedBoundingBoxWithVisualization(wrapedGraphics->GetModel()->GetVertices());

	trianglesColliderVisualization = GraphicsObjectManager::CreateGO3DColored(const_cast<Model*>(wrapedGraphics->GetModel()), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

StaticCollider::~StaticCollider()
{
	GraphicsObjectManager::Delete(trianglesColliderVisualization);
}

void StaticCollider::Update()
{
	boundingSphere->Update(wrapedGraphics->GetTransform());
	obb->Update(wrapedGraphics->GetTransform());
	trianglesColliderVisualization->SetTransform(wrapedGraphics->GetTransform());
}

void StaticCollider::ToggleVisibility()
{
}

bool StaticCollider::Intersect(const OrientedBoundingBox& other) const
{
	return false;
}

bool StaticCollider::Intersect(const LineSegment3D& other) const
{
	return false;
}

bool StaticCollider::Intersect(const AnimatedCollider& other) const
{
	return other.Intersect(*this);
}

const OrientedBoundingBoxWithVisualization* const StaticCollider::GetBox() const
{
	return obb;
}

const SphereWithVisualization* const StaticCollider::GetSphere() const
{
	return boundingSphere;
}

const Model* const StaticCollider::GetModel() const
{
	return wrapedGraphics->GetModel();
}

glm::mat4 StaticCollider::GetTransform() const
{
	return wrapedGraphics->GetTransform();
}
