#include "StaticCollider.h"

#include "../Collision/OrientedBoundingBoxWithVisualization.h"
#include "../Collision/AnimatedCollider.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Collision/SphereWithVisualization.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Math/Shapes/Triangle.h"
#include "../Math/Shapes/Ray.h"
#include "../Math/Shapes/LineSegment3D.h"
#include "../Utils/Logger.h"

#include <list>
#include <algorithm>
#include <execution>
#include <atomic>

StaticCollider::StaticCollider(GO3D* const graphicsObject) :
	wrapedGraphics(graphicsObject)
{
	this->boundingSphere = new SphereWithVisualization(wrapedGraphics);
	this->obb = new OrientedBoundingBoxWithVisualization(wrapedGraphics->GetModel()->GetVertices());

	trianglesColliderVisualization = GraphicsObjectManager::CreateGO3DColored(const_cast<Model*>(wrapedGraphics->GetModel()), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	trianglesColliderVisualization->SetDrawMode(GO3D::Mode::LINE);

	AccelerateMesh();

	AxisAlignedBoundingBoxWithVisualization::UpdateInstanceTransforms();
}

StaticCollider::~StaticCollider()
{
	GraphicsObjectManager::Delete(trianglesColliderVisualization);
	delete obb;
	delete boundingSphere;

	if (accelerator != nullptr)
	{
		FreeBVHNode(accelerator);
		delete accelerator;
	}
}

void StaticCollider::Update()
{
	boundingSphere->Update(wrapedGraphics->GetTransform());
	obb->Update(wrapedGraphics->GetTransform());
	trianglesColliderVisualization->SetTransform(wrapedGraphics->GetTransform());


	if (accelerator != nullptr)
	{
		//Updated instanced visualized aabbs
		//Recursively walk the BVH tree.
		std::list<BVHNode*> toProcess;
		toProcess.push_front(accelerator);
		while (!toProcess.empty())
		{
			BVHNode* iterator = *(toProcess.begin());
			toProcess.erase(toProcess.begin());

			iterator->bounds.UpdateGraphicsInstance();

			if (iterator->children != 0)
			{
				for (int i = 8 - 1; i >= 0; --i)
				{
					toProcess.push_front(&iterator->children[i]);
				}
			}
		}
	}
}

void StaticCollider::ToggleVisibility()
{
	this->boundingSphere->ToggleVisibility();
	this->obb->ToggleVisibility();
	if (trianglesColliderVisualization->IsDisabled())
	{
		GraphicsObjectManager::Enable(trianglesColliderVisualization);
	}
	else
	{
		GraphicsObjectManager::Disable(trianglesColliderVisualization);
	}

	visible = !visible;
}

bool StaticCollider::Intersect(const OrientedBoundingBox& other) const
{
	return false;
}

bool StaticCollider::Intersect(const LineSegment3D& other) const
{
	if (boundingSphere->LineSegmentIntersect(other) && GetBox()->LineIntersect(other))
	{
		Ray ray(other.GetStart(), glm::normalize(other.GetEnd() - other.GetStart()));
		return Intersect(ray) != -1.0f;
	}

	return false;
}

bool StaticCollider::Intersect(const AnimatedCollider& other) const
{
	return other.Intersect(*this);
}

float StaticCollider::Intersect(const Ray& ray) const
{
	const std::vector<Vertex> vertices = wrapedGraphics->GetModel()->GetVertices();
	const std::vector<unsigned int> indices = wrapedGraphics->GetModel()->GetIndices();

	struct TriangleIndices
	{
		unsigned int triIndex;
		unsigned int a;
		unsigned int b;
		unsigned int c;
	};
	
	std::vector<TriangleIndices> trianglesIndices;

	unsigned int triIndex = 0;
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		trianglesIndices.push_back({ triIndex++, indices[i], indices[i + 1], indices[i + 2] });
	}

	std::vector<Triangle> triangles(indices.size()/3);

	std::for_each(std::execution::par, trianglesIndices.begin(), trianglesIndices.end(), [&triangles, &vertices, this](const TriangleIndices& triangleIndices)
		{
			triangles[triangleIndices.triIndex] = Triangle(wrapedGraphics->GetTransform() * glm::vec4(vertices[triangleIndices.a].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[triangleIndices.b].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[triangleIndices.c].GetPosition(), 1.0f));
		});

	//for (unsigned int i = 0; i < indices.size(); i += 3)
	//{
	//	Triangle t(wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i]].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i + 1]].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i + 2]].GetPosition(), 1.0f));
	//	triangles.push_back(t);
	//}


	std::atomic<float> result = -1.0f;
	std::for_each(std::execution::par, triangles.begin(), triangles.end(), [&result, &ray](const Triangle& tri)
		{
			float currentResult = tri.Raycast(ray);
			if (currentResult >= 0)
			{
				if (currentResult < result.load())
				{
					result.store(currentResult);
				}
				else if(result.load() == -1.0f)
				{
					result.store(currentResult);
				}
			}
		});
	
	return result.load();

	//if (accelerator == nullptr)
	//{
	//
	//	
	//}
	//else
	//{
	//	std::list<BVHNode*> toProcess;
	//	toProcess.push_front(accelerator);
	//
	//	//Recursively walk the BVH tree.
	//	while (!toProcess.empty())
	//	{
	//		BVHNode* iterator = *(toProcess.begin());
	//		toProcess.erase(toProcess.begin());
	//
	//		if (iterator->numTriangles >= 0)
	//		{
	//			for (int i = 0; i < iterator->numTriangles; ++i)
	//			{
	//				float r = triangles[iterator->triangles[i]].Raycast(ray);
	//				if (r >= 0)
	//				{
	//					return r;
	//				}
	//			}
	//		}
	//
	//		if (iterator->children != 0)
	//		{
	//			for (int i = 8 - 1; i >= 0; --i)
	//			{
	//				if (iterator->children[i].bounds.RayIntersect(ray) >= 0)
	//				{
	//					toProcess.push_front(&iterator->children[i]);
	//				}
	//			}
	//		}
	//	}
	//}

	return -1.0f;
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

void StaticCollider::Translate(const glm::vec3& translation)
{
	std::list<BVHNode*> toProcess;
	toProcess.push_front(accelerator);

	//Recursively walk the BVH tree.
	while (!toProcess.empty())
	{
		BVHNode* iterator = *(toProcess.begin());
		toProcess.erase(toProcess.begin());

		iterator->bounds.FromOriginAndSize(iterator->bounds.GetOrigin() + translation, iterator->bounds.GetSize());
		iterator->bounds.Update();

		if (iterator->children != 0)
		{
			for (int i = 8 - 1; i >= 0; --i)
			{
				toProcess.push_front(&iterator->children[i]);
			}
		}
	}
}

const Model* const StaticCollider::GetWrapedGraphicsModel() const
{
	return wrapedGraphics->GetModel();
}

std::vector<Triangle> StaticCollider::GetTriangles() const
{
	const std::vector<Vertex> vertices = wrapedGraphics->GetModel()->GetVertices();
	const std::vector<unsigned int> indices = wrapedGraphics->GetModel()->GetIndices();

	std::vector<Triangle> triangles;

	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		Triangle t(wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i]].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i + 1]].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i + 2]].GetPosition(), 1.0f));
		triangles.push_back(t);
	}


	return triangles;
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
	accelerator->numTriangles = static_cast<int>(vertices.size()) / 3U;
	accelerator->triangles = new int[accelerator->numTriangles]();

	for (int i = 0; i < accelerator->numTriangles; ++i)
	{
		accelerator->triangles[i] = i;
	}

	SplitBVHNode(accelerator, 2);
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
		const std::vector<Vertex> vertices = wrapedGraphics->GetModel()->GetVertices();
		const std::vector<unsigned int> indices = wrapedGraphics->GetModel()->GetIndices();

		std::vector<Triangle> triangles;

		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			Triangle t(wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i]].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i + 1]].GetPosition(), 1.0f), wrapedGraphics->GetTransform() * glm::vec4(vertices[indices[i + 2]].GetPosition(), 1.0f));
			triangles.push_back(t);
		}

		for (unsigned int i = 0; i < 8; ++i)
		{
			node->children[i].numTriangles = 0;
			for (int j = 0; j < node->numTriangles; ++j)
			{
				const Triangle& t = triangles[node->triangles[j]];

				if (t.AxisAlignedBoundingBoxIntersect(node->children[i].bounds))
				{
					node->children[i].numTriangles += 1;
				}
			}

			if (node->children[i].numTriangles == 0)
			{
				continue;
			}

			node->children[i].triangles = new int[node->children[i].numTriangles];

			int index = 0;
			for (int j = 0; j < node->numTriangles; ++j)
			{
				const Triangle& t = triangles[node->triangles[j]];
				if (t.AxisAlignedBoundingBoxIntersect(node->children[i].bounds))
				{
					node->children[i].triangles[index++] = node->triangles[j];
				}
			}
		}

		node->numTriangles = 0;
		delete[] node->triangles;
		node->triangles = 0;

		for (int i = 0; i < 8; ++i)
		{
			SplitBVHNode(&node->children[i], depth);
		}
	}
}

void StaticCollider::FreeBVHNode(BVHNode* node)
{
	if (node != nullptr)
	{
		if (node->children != nullptr)
		{
			for (int i = 0; i < 8; ++i)
			{
				FreeBVHNode(&node->children[i]);
			}
			delete[] node->children;
			node->children = 0;
		}

		if (node->numTriangles != 0 || node->triangles != nullptr)
		{
			delete[] node->triangles;
			node->triangles = nullptr;
			node->numTriangles = 0;
		}
	}
}
