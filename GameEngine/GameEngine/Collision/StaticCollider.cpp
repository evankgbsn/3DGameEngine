#include "StaticCollider.h"

#include "../Collision/OrientedBoundingBoxWithVisualization.h"
#include "../Collision/AnimatedCollider.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/GraphicsObjects/GOTexturedLitInstanced.h"
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
	wrapedGraphics(graphicsObject),
	lastTranslation(graphicsObject->GetTranslation()),
	wrapedGraphicsInstanced(nullptr)
{
	this->boundingSphere = new SphereWithVisualization(wrapedGraphics);
	this->obb = new OrientedBoundingBoxWithVisualization(wrapedGraphics->GetModel()->GetVertices());

	trianglesColliderVisualization = GraphicsObjectManager::CreateGO3DColored(const_cast<Model*>(wrapedGraphics->GetModel()), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	trianglesColliderVisualization->SetDrawMode(GO3D::Mode::LINE);
}

StaticCollider::StaticCollider(GOTexturedLitInstanced* const graphicsObjectInstanced, unsigned int instanceID)	:
	wrapedGraphicsInstanced(graphicsObjectInstanced),
	lastTranslation(graphicsObjectInstanced->GetTranslation(instanceID)),
	wrapedGraphics(nullptr)
{
	this->boundingSphere = new SphereWithVisualization(wrapedGraphics);
	this->obb = new OrientedBoundingBoxWithVisualization(wrapedGraphics->GetModel()->GetVertices());

	trianglesColliderVisualization = GraphicsObjectManager::CreateGO3DColored(const_cast<Model*>(wrapedGraphics->GetModel()), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	trianglesColliderVisualization->SetDrawMode(GO3D::Mode::LINE);
}

StaticCollider::~StaticCollider()
{
	GraphicsObjectManager::Delete(trianglesColliderVisualization);
	delete obb;
	delete boundingSphere;
}

void StaticCollider::Update()
{
	if (wrapedGraphicsInstanced == nullptr)
	{
		boundingSphere->Update(wrapedGraphics->GetTransform());
		obb->Update(wrapedGraphics->GetTransform());
		trianglesColliderVisualization->SetTransform(wrapedGraphics->GetTransform());

		glm::vec3 translation = wrapedGraphics->GetTranslation() - lastTranslation;

		lastTranslation = wrapedGraphics->GetTranslation();
	}
	else
	{
		boundingSphere->Update(wrapedGraphicsInstanced->GetTransform(instanceID));
		obb->Update(wrapedGraphicsInstanced->GetTransform(instanceID));
		trianglesColliderVisualization->SetTransform(wrapedGraphicsInstanced->GetTransform(instanceID));

		glm::vec3 translation = wrapedGraphicsInstanced->GetTranslation(instanceID) - lastTranslation;

		lastTranslation = wrapedGraphicsInstanced->GetTranslation(instanceID);
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
		glm::vec3 lineVector = other.GetEnd() - other.GetStart();
		glm::vec3 direction = glm::normalize(lineVector);
		Ray ray(other.GetStart(), direction);
		float intersectPoint = Intersect(ray);
		return intersectPoint != -1.0f && glm::length(lineVector) >= glm::length(other.GetEnd() - (other.GetStart() + direction * intersectPoint));
	}

	return false;
}

bool StaticCollider::Intersect(const AnimatedCollider& other, glm::vec3& outHit) const
{
	return other.Intersect(*this, outHit);
}

float StaticCollider::Intersect(const Ray& ray) const
{

	if (wrapedGraphicsInstanced == nullptr)
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

		std::vector<Triangle> triangles(indices.size() / 3);

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
					else if (result.load() == -1.0f)
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
	else
	{
		const std::vector<Vertex> vertices = wrapedGraphicsInstanced->GetModel()->GetVertices();
		const std::vector<unsigned int> indices = wrapedGraphicsInstanced->GetModel()->GetIndices();

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

		std::vector<Triangle> triangles(indices.size() / 3);

		std::for_each(std::execution::par, trianglesIndices.begin(), trianglesIndices.end(), [&triangles, &vertices, this](const TriangleIndices& triangleIndices)
			{
				triangles[triangleIndices.triIndex] = Triangle(wrapedGraphicsInstanced->GetTransform(instanceID) * glm::vec4(vertices[triangleIndices.a].GetPosition(), 1.0f), wrapedGraphicsInstanced->GetTransform(instanceID) * glm::vec4(vertices[triangleIndices.b].GetPosition(), 1.0f), wrapedGraphicsInstanced->GetTransform(instanceID) * glm::vec4(vertices[triangleIndices.c].GetPosition(), 1.0f));
			});

		//for (unsigned int i = 0; i < indices.size(); i += 3)
		//{
		//	Triangle t(wrapedGraphicsInstanced->GetTransform() * glm::vec4(vertices[indices[i]].GetPosition(), 1.0f), wrapedGraphicsInstanced->GetTransform() * glm::vec4(vertices[indices[i + 1]].GetPosition(), 1.0f), wrapedGraphicsInstanced->GetTransform() * glm::vec4(vertices[indices[i + 2]].GetPosition(), 1.0f));
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
					else if (result.load() == -1.0f)
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
	if (wrapedGraphicsInstanced == nullptr)
	{
		return wrapedGraphics->GetModel();
	}
	else
	{
		return wrapedGraphicsInstanced->GetModel();
	}
}

glm::mat4 StaticCollider::GetTransform() const
{
	if (wrapedGraphicsInstanced == nullptr)
	{
		return wrapedGraphics->GetTransform();
	}
	else
	{
		return wrapedGraphicsInstanced->GetTransform(instanceID);
	}
}

const Model* const StaticCollider::GetWrapedGraphicsModel() const
{
	if (wrapedGraphicsInstanced == nullptr)
	{
		return wrapedGraphics->GetModel();
	}
	else
	{
		return wrapedGraphicsInstanced->GetModel();
	}
}

std::vector<Triangle> StaticCollider::GetTriangles() const
{
	if (wrapedGraphicsInstanced == nullptr)
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
	else
	{
		const std::vector<Vertex> vertices = wrapedGraphicsInstanced->GetModel()->GetVertices();
		const std::vector<unsigned int> indices = wrapedGraphicsInstanced->GetModel()->GetIndices();

		std::vector<Triangle> triangles;

		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			Triangle t(wrapedGraphicsInstanced->GetTransform(instanceID) * glm::vec4(vertices[indices[i]].GetPosition(), 1.0f), wrapedGraphicsInstanced->GetTransform(instanceID) * glm::vec4(vertices[indices[i + 1]].GetPosition(), 1.0f), wrapedGraphicsInstanced->GetTransform(instanceID) * glm::vec4(vertices[indices[i + 2]].GetPosition(), 1.0f));
			triangles.push_back(t);
		}


		return triangles;
	}
	
}

glm::vec3 StaticCollider::GetSphereOrigin() const
{
	return boundingSphere->GetOrigin();
}

glm::vec3 StaticCollider::GetBoxOrigin() const
{
	return obb->GetOrigin();
}