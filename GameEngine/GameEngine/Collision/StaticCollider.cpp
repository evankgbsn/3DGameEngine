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

	this->obb->ToggleVisibility();

	trianglesColliderVisualization = GraphicsObjectManager::CreateGO3DColored(const_cast<Model*>(wrapedGraphics->GetModel()), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	AccelerateMesh();
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

void StaticCollider::AccelerateMesh()
{
	if (accelerator != nullptr)
	{
		return;
	}

	const std::vector<Vertex>& vertices = wrapedGraphics->GetModel()->GetVertices();

	glm::vec3 min = vertices[0].GetPosition();
	glm::vec3 max = vertices[0].GetPosition();

	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		min.x = fminf(vertices[i].GetPosition().x, min.x);
		min.y = fminf(vertices[i].GetPosition().y, min.y);
		min.z = fminf(vertices[i].GetPosition().z, min.z);
		max.x = fmaxf(vertices[i].GetPosition().x, max.x);
		max.y = fmaxf(vertices[i].GetPosition().y, max.y);
		max.z = fmaxf(vertices[i].GetPosition().z, max.z);
	}

	accelerator = new BVHNode();
	accelerator->bounds.FromMinAndMax(wrapedGraphics->GetTransform() * glm::vec4(min, 1.0f), wrapedGraphics->GetTransform() *glm::vec4( max, 1.0f));
	accelerator->bounds.Update();
	accelerator->bounds.ToggleVisibility();
	accelerator->numTriangles = vertices.size() / 3;
	accelerator->triangles = new int[accelerator->numTriangles]();

	for (unsigned int i = 0; i < accelerator->numTriangles; ++i)
	{
		accelerator->triangles[i] = i;
	}

	SplitBVHNode(accelerator, 3);
}

void StaticCollider::SplitBVHNode(BVHNode* node, int depth)
{
	if (depth-- == 0)
	{
		return;
	}

	if (node->children == nullptr)
	{
		if (node->numTriangles > 0)
		{
			node->children = new BVHNode[8];

			vec3 c = node->bounds.GetOrigin();
			vec3 e = node->bounds.GetSize() * 0.5f;

			node->children[0].bounds.FromOriginAndSize(c + vec3(-e.x, +e.y, -e.z), e);
			node->children[0].bounds.Update();
			node->children[0].bounds.ToggleVisibility();
			node->children[1].bounds.FromOriginAndSize(c + vec3(+e.x, +e.y, -e.z), e);
			node->children[1].bounds.Update();
			node->children[1].bounds.ToggleVisibility();
			node->children[2].bounds.FromOriginAndSize(c + vec3(-e.x, +e.y, +e.z), e);
			node->children[2].bounds.Update();
			node->children[2].bounds.ToggleVisibility();
			node->children[3].bounds.FromOriginAndSize(c + vec3(+e.x, +e.y, +e.z), e);
			node->children[3].bounds.Update();
			node->children[3].bounds.ToggleVisibility();
			node->children[4].bounds.FromOriginAndSize(c + vec3(-e.x, -e.y, -e.z), e);
			node->children[4].bounds.Update();
			node->children[4].bounds.ToggleVisibility();
			node->children[5].bounds.FromOriginAndSize(c + vec3(+e.x, -e.y, -e.z), e);
			node->children[5].bounds.Update();
			node->children[5].bounds.ToggleVisibility();
			node->children[6].bounds.FromOriginAndSize(c + vec3(-e.x, -e.y, +e.z), e);
			node->children[6].bounds.Update();
			node->children[6].bounds.ToggleVisibility();
			node->children[7].bounds.FromOriginAndSize(c + vec3(+e.x, -e.y, +e.z), e);
			node->children[7].bounds.Update();
			node->children[7].bounds.ToggleVisibility();
		}
	}

	if (node->children != 0 && node->numTriangles > 0)
	{
		for (unsigned int i = 0; i < 8; ++i)
		{

		}
	}
}
