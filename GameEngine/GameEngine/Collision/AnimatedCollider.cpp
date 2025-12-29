#include "AnimatedCollider.h"

#include "OrientedBoundingBoxWithVisualization.h"
#include "SphereWithVisualization.h"
#include "../Renderer/GraphicsObjects/GO3DAnimated.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Model/Vertex.h"
#include "../Renderer/Animation/Armature.h"
#include "../Renderer/GraphicsObjects/GOColoredAnimated.h"
#include "../Renderer/GraphicsObjects/GOColored.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Math/Shapes/LineSegment3D.h"
#include "../Math/Shapes/Triangle.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Collision/StaticCollider.h"

#include <execution>
#include <mutex>

AnimatedCollider::AnimatedCollider(GO3DAnimated* const graphicsObject) :
	wrapedGraphics(graphicsObject)
{
	InitializeSphere();
	InitializeOBBs();
	InitializeMeshColliderVisualization();
}

AnimatedCollider::~AnimatedCollider()
{
	for (const std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>& obb : obbs)
	{
		delete obb.first;
	}

	GraphicsObjectManager::Delete(meshColliderVisualization);
	delete sphere;
}

glm::mat4 AnimatedCollider::GetJointTransform(const std::string& jointName) const
{
	for (unsigned int i = 0; i < jointNames->size(); i++)
	{
		if (jointName == (*jointNames)[i])
		{
			return wrapedGraphics->GetTransform() * wrapedGraphics->GetAnimPoseArray()[i];
		}
	}

	return glm::mat4(1.0f);
}

void AnimatedCollider::InitializeOBBs()
{
	arms = wrapedGraphics->GetModel()->GetArmature();

	jointNames = &arms->GetJointNames();

	const std::vector<Vertex>& vertices = wrapedGraphics->GetModel()->GetVertices();

	const std::vector<unsigned int>& indices = wrapedGraphics->GetModel()->GetIndices();

	unsigned int loopCount = static_cast<unsigned int>(indices.size() - 4);

	for (unsigned int i = 0; i < loopCount; i += 3)
	{
		std::vector<Vertex> triangle = { vertices[indices[i]], vertices[indices[i + 1 ]], vertices[indices[i + 2]]};

		unsigned int triangleJointInfluence = 0;
		if (triangle[0].GetInfluences()[0] == triangle[1].GetInfluences()[0] || triangle[0].GetInfluences()[0] == triangle[2].GetInfluences()[0])
		{
			triangleJointInfluence = triangle[0].GetInfluences()[0];
		}
		else
		{
			triangleJointInfluence = triangle[0].GetInfluences()[0];
		}

		jointsTriangles[jointNames->at(triangleJointInfluence)].push_back(triangle);
	}

	for (unsigned int i = 0; i < jointNames->size(); ++i)
	{
		obbs.push_back(std::make_pair(nullptr, 0));
	}
	
	for (const std::pair<std::string, std::vector<std::vector<Vertex>>>& jointVerts : jointsTriangles)
	{
		std::vector<Vertex> verts;

		for (const std::vector<Vertex>& vert : jointVerts.second)
		{
			for (const Vertex& v : vert)
			{
				verts.push_back(v);
			}
		}

		OrientedBoundingBoxWithVisualization* newObb = new OrientedBoundingBoxWithVisualization(verts);
	
		for (unsigned int i = 0; i < jointNames->size(); ++i)
		{
			if (jointNames->at(i) == jointVerts.first)
			{
				obbs[i] = std::make_pair(newObb, 0);
			}
		}

		for (unsigned int i = 0; i < obbs.size(); i++)
		{
			obbs[i].second = i;
		}
	}

	
}

void AnimatedCollider::InitializeSphere()
{
	sphere = new SphereWithVisualization(wrapedGraphics);
}

void AnimatedCollider::InitializeMeshColliderVisualization()
{
	meshColliderVisualization = GraphicsObjectManager::CreateGO3DColoredAnimated(const_cast<Model* const>(wrapedGraphics->GetModel()), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	meshColliderVisualization->SetDrawMode(GO3D::Mode::LINE);
	meshColliderVisualization->SetClip(wrapedGraphics->GetClipName());
	meshColliderVisualization->SetTransform(wrapedGraphics->GetTransform());
	meshColliderVisualization->SetSampleTime(wrapedGraphics->GetSampleTime());
	meshColliderVisualization->SetSpeed(wrapedGraphics->GetSpeed());
}

void AnimatedCollider::ToggleVisibility()
{
	for (const auto& obb : obbs)
	{
		if(obb.first != nullptr)
			obb.first->ToggleVisibility();
	}

	sphere->ToggleVisibility();

	if (visible)
	{
		GraphicsObjectManager::Disable(meshColliderVisualization);
	}
	else
	{
		GraphicsObjectManager::Enable(meshColliderVisualization);
		meshColliderVisualization->SetSampleTime(wrapedGraphics->GetSampleTime());
		meshColliderVisualization->SetSpeed(wrapedGraphics->GetSpeed());
	}
		

	visible = !visible;
}

void AnimatedCollider::Update()
{
	unsigned int i = 0;
	for (const std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>& obb : obbs)
	{
		if(obb. first != nullptr)
			obb.first->Update(wrapedGraphics->GetTransform() * wrapedGraphics->GetAnimPoseArray()[i]);
		i++;
	}

	meshColliderVisualization->SetTransform(wrapedGraphics->GetTransform());
	meshColliderVisualization->SetSampleTime(wrapedGraphics->GetSampleTime());

	sphere->Update(wrapedGraphics->GetTransform());
}

bool AnimatedCollider::Intersect(const OrientedBoundingBox& other) const
{
	bool intersect = false;
	for (const std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>& obb : obbs)
	{
		if (obb.first != nullptr)
		{
			if (obb.first->OrientedBoundingBoxIntersect(other))
			{
				obb.first->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				intersect = true;
			}
			else
			{
				obb.first->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
			}
			
		}
	}

	return intersect;
}

bool AnimatedCollider::Intersect(const LineSegment3D& other) const
{
	bool intersect = false;

	std::for_each(std::execution::par, obbs.begin(), obbs.end(),
		[this, &other, &intersect](const std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>& obb)
		{
			if (obb.first != nullptr)
			{
				if (obb.first->LineIntersect(other))
				{
					const std::vector<std::vector<Vertex>>& obbTriangles = jointsTriangles.find(jointNames->at(obb.second))->second;

					std::for_each(std::execution::par, obbTriangles.begin(), obbTriangles.end(), [this, &other, &intersect](const std::vector<Vertex>& triangleVerts)
						{
							auto skinVertexForTriangleCollider = [this, &other, &intersect](const Vertex& vert, GO3DAnimated* animatedVisualization) -> glm::mat4
								{
									const glm::ivec4& influences = vert.GetInfluences();

									const glm::mat4* const animPoseMatArray = animatedVisualization->GetAnimPoseArray();
									glm::mat4 animPoseJoint0 = animPoseMatArray[influences[0]];
									glm::mat4 animPoseJoint1 = animPoseMatArray[influences[1]];
									glm::mat4 animPoseJoint2 = animPoseMatArray[influences[2]];
									glm::mat4 animPoseJoint3 = animPoseMatArray[influences[3]];

									const std::vector<glm::mat4>& animInvBindPoseMatArray = animatedVisualization->GetAnimInvBindPoseArray();
									glm::mat4 animInvBindPoseJoint0 = animInvBindPoseMatArray[influences[0]];
									glm::mat4 animInvBindPoseJoint1 = animInvBindPoseMatArray[influences[1]];
									glm::mat4 animInvBindPoseJoint2 = animInvBindPoseMatArray[influences[2]];
									glm::mat4 animInvBindPoseJoint3 = animInvBindPoseMatArray[influences[3]];

									const glm::vec4& weights = vert.GetWeights();

									glm::mat4 skin = animPoseJoint0 * animInvBindPoseJoint0 * weights[0];
									skin += animPoseJoint1 * animInvBindPoseJoint1 * weights[1];
									skin += animPoseJoint2 * animInvBindPoseJoint2 * weights[2];
									skin += animPoseJoint3 * animInvBindPoseJoint3 * weights[3];

									return skin;
								};

							Triangle t1(
								wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[0], wrapedGraphics) * glm::vec4(triangleVerts[0].GetPosition(), 1.0f),
								wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[1], wrapedGraphics) * glm::vec4(triangleVerts[1].GetPosition(), 1.0f),
								wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[2], wrapedGraphics) * glm::vec4(triangleVerts[2].GetPosition(), 1.0f));

							if (t1.LineIntersect(other))
							{
								intersect = true;

							}

						});
				}
			}
		});

	if (intersect)
	{
		meshColliderVisualization->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		meshColliderVisualization->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	}

	return intersect;
}

bool AnimatedCollider::Intersect(const AnimatedCollider& other) const
{
	bool intersect = false;

	static std::mutex colorMutex;

	std::vector<std::function<void()>> colorFunctions;

	if (sphere->SphereIntersect(*other.sphere))
	{
		sphere->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		std::for_each(std::execution::par, obbs.begin(), obbs.end(),
			[this, &other, &intersect, &colorFunctions](const std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>& obb)
			{
				std::for_each(std::execution::par, other.obbs.begin(), other.obbs.end(),
				[this, &other, &intersect, obb, &colorFunctions](const std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>& otherObb)
					{
						if (obb.first != nullptr && otherObb.first != nullptr)
						{
							if (otherObb.first->OrientedBoundingBoxIntersect(*obb.first))
							{
								const std::vector<std::vector<Vertex>>& obbTriangles = jointsTriangles.find(jointNames->at(obb.second))->second;
								const std::vector<std::vector<Vertex>>& otherObbTriangles = other.jointsTriangles.find(jointNames->at(otherObb.second))->second;

								std::for_each(std::execution::par, obbTriangles.begin(), obbTriangles.end(),
									[this, &otherObbTriangles, &other, &intersect](const std::vector<Vertex>& triangleVerts)
									{
										std::for_each(std::execution::par, otherObbTriangles.begin(), otherObbTriangles.end(),
										[this, triangleVerts, &other, &intersect](const std::vector<Vertex>& otherTriangleVerts)
											{
												auto skinVertexForTriangleCollider = [this](const Vertex& vert, GO3DAnimated* animatedVisualization) -> glm::mat4
													{
														const glm::ivec4& influences = vert.GetInfluences();

														const glm::mat4* const animPoseMatArray = animatedVisualization->GetAnimPoseArray();
														glm::mat4 animPoseJoint0 = animPoseMatArray[influences[0]];
														glm::mat4 animPoseJoint1 = animPoseMatArray[influences[1]];
														glm::mat4 animPoseJoint2 = animPoseMatArray[influences[2]];
														glm::mat4 animPoseJoint3 = animPoseMatArray[influences[3]];

														const std::vector<glm::mat4>& const animInvBindPoseMatArray = animatedVisualization->GetAnimInvBindPoseArray();
														glm::mat4 animInvBindPoseJoint0 = animInvBindPoseMatArray[influences[0]];
														glm::mat4 animInvBindPoseJoint1 = animInvBindPoseMatArray[influences[1]];
														glm::mat4 animInvBindPoseJoint2 = animInvBindPoseMatArray[influences[2]];
														glm::mat4 animInvBindPoseJoint3 = animInvBindPoseMatArray[influences[3]];

														const glm::vec4& weights = vert.GetWeights();

														glm::mat4 skin = animPoseJoint0 * animInvBindPoseJoint0 * weights[0];
														skin += animPoseJoint1 * animInvBindPoseJoint1 * weights[1];
														skin += animPoseJoint2 * animInvBindPoseJoint2 * weights[2];
														skin += animPoseJoint3 * animInvBindPoseJoint3 * weights[3];

														return skin;
													};

												Triangle t1(
													wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[0], wrapedGraphics) * glm::vec4(triangleVerts[0].GetPosition(), 1.0f),
													wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[1], wrapedGraphics) * glm::vec4(triangleVerts[1].GetPosition(), 1.0f),
													wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[2], wrapedGraphics) * glm::vec4(triangleVerts[2].GetPosition(), 1.0f));

												Triangle t2(
													other.wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(otherTriangleVerts[0], other.wrapedGraphics) * glm::vec4(otherTriangleVerts[0].GetPosition(), 1.0f),
													other.wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(otherTriangleVerts[1], other.wrapedGraphics) * glm::vec4(otherTriangleVerts[1].GetPosition(), 1.0f),
													other.wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(otherTriangleVerts[2], other.wrapedGraphics) * glm::vec4(otherTriangleVerts[2].GetPosition(), 1.0f));

												if (t1.TriangleIntersectRobust(t2))
												{
													intersect = true;
													meshColliderVisualization->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
												}
											});
									});

								

								{
									std::lock_guard<std::mutex> guard(colorMutex);
									const glm::vec4 colorRed = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
									if (obb.first->GetColor() != colorRed)
									{
										std::function<void()> setColorRedFromMainThread = [obb, colorRed]()
											{
												if (obb.first->GetColor() != colorRed)
												{
													obb.first->SetColor(colorRed);
												}
											};

										colorFunctions.push_back(setColorRedFromMainThread);
									}
								}
							}
							else
							{
								std::lock_guard<std::mutex> guard(colorMutex);
								const glm::vec4 colorGreen = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
								if (obb.first->GetColor() != colorGreen)
								{
									std::function<void()> setColorRedFromMainThread = [obb, colorGreen]()
										{
											if (obb.first->GetColor() != colorGreen)
											{
												obb.first->SetColor(colorGreen);
											}
										};

									colorFunctions.push_back(setColorRedFromMainThread);
								}
							}

						}
					});
			});

			for (auto& colorFunc : colorFunctions)
			{
				colorFunc();
			}

		if (intersect == false)
		{
			meshColliderVisualization->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
	else
	{
		sphere->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	}

	return intersect;
}

bool AnimatedCollider::Intersect(const StaticCollider& other, glm::vec3& outHit) const
{
	if (!sphere->SphereIntersect(*other.GetSphere()))
	{
		return false;
	}

	bool obbIntersect = false;

	for (const auto& obb : obbs)
	{
		if (obb.first == nullptr)
		{
			continue;
		}

		if (obb.first->OrientedBoundingBoxIntersect(*other.GetBox()))
		{
			const std::vector<std::vector<Vertex>>& obbTriangles = jointsTriangles.find(jointNames->at(obb.second))->second;

			for (const auto& triangleVerts : obbTriangles)
			{
				auto skinVertexForTriangleCollider = [this, &other](const Vertex& vert, GO3DAnimated* animatedVisualization) -> glm::mat4
					{
						const glm::ivec4& influences = vert.GetInfluences();

						const glm::mat4* const animPoseMatArray = animatedVisualization->GetAnimPoseArray();
						glm::mat4 animPoseJoint0 = animPoseMatArray[influences[0]];
						glm::mat4 animPoseJoint1 = animPoseMatArray[influences[1]];
						glm::mat4 animPoseJoint2 = animPoseMatArray[influences[2]];
						glm::mat4 animPoseJoint3 = animPoseMatArray[influences[3]];

						const std::vector<glm::mat4>& animInvBindPoseMatArray = animatedVisualization->GetAnimInvBindPoseArray();
						glm::mat4 animInvBindPoseJoint0 = animInvBindPoseMatArray[influences[0]];
						glm::mat4 animInvBindPoseJoint1 = animInvBindPoseMatArray[influences[1]];
						glm::mat4 animInvBindPoseJoint2 = animInvBindPoseMatArray[influences[2]];
						glm::mat4 animInvBindPoseJoint3 = animInvBindPoseMatArray[influences[3]];

						const glm::vec4& weights = vert.GetWeights();

						glm::mat4 skin = animPoseJoint0 * animInvBindPoseJoint0 * weights[0];
						skin += animPoseJoint1 * animInvBindPoseJoint1 * weights[1];
						skin += animPoseJoint2 * animInvBindPoseJoint2 * weights[2];
						skin += animPoseJoint3 * animInvBindPoseJoint3 * weights[3];

						return skin;
					};

				Triangle t1(
					wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[0], wrapedGraphics) * glm::vec4(triangleVerts[0].GetPosition(), 1.0f),
					wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[1], wrapedGraphics) * glm::vec4(triangleVerts[1].GetPosition(), 1.0f),
					wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[2], wrapedGraphics) * glm::vec4(triangleVerts[2].GetPosition(), 1.0f));


				static const std::vector<Triangle> otherTriangles = other.GetTriangles();

				for (int i = 0; i < otherTriangles.size(); ++i)
				{
					if (t1.TriangleIntersect(otherTriangles[i], outHit))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool AnimatedCollider::Intersect(const Ray& other) const
{
	bool intersect = false;

	std::for_each(std::execution::par, obbs.begin(), obbs.end(),
		[this, &other, &intersect](const std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>& obb)
		{
			if (obb.first != nullptr)
			{
				if (obb.first->RayIntersect(other) != -1.0f)
				{
					const std::vector<std::vector<Vertex>>& obbTriangles = jointsTriangles.find(jointNames->at(obb.second))->second;

					std::for_each(std::execution::par, obbTriangles.begin(), obbTriangles.end(), [this, &other, &intersect](const std::vector<Vertex>& triangleVerts)
						{
							auto skinVertexForTriangleCollider = [this, &other, &intersect](const Vertex& vert, GO3DAnimated* animatedVisualization) -> glm::mat4
								{
									const glm::ivec4& influences = vert.GetInfluences();

									const glm::mat4* const animPoseMatArray = animatedVisualization->GetAnimPoseArray();
									glm::mat4 animPoseJoint0 = animPoseMatArray[influences[0]];
									glm::mat4 animPoseJoint1 = animPoseMatArray[influences[1]];
									glm::mat4 animPoseJoint2 = animPoseMatArray[influences[2]];
									glm::mat4 animPoseJoint3 = animPoseMatArray[influences[3]];

									const std::vector<glm::mat4>& animInvBindPoseMatArray = animatedVisualization->GetAnimInvBindPoseArray();
									glm::mat4 animInvBindPoseJoint0 = animInvBindPoseMatArray[influences[0]];
									glm::mat4 animInvBindPoseJoint1 = animInvBindPoseMatArray[influences[1]];
									glm::mat4 animInvBindPoseJoint2 = animInvBindPoseMatArray[influences[2]];
									glm::mat4 animInvBindPoseJoint3 = animInvBindPoseMatArray[influences[3]];

									const glm::vec4& weights = vert.GetWeights();

									glm::mat4 skin = animPoseJoint0 * animInvBindPoseJoint0 * weights[0];
									skin += animPoseJoint1 * animInvBindPoseJoint1 * weights[1];
									skin += animPoseJoint2 * animInvBindPoseJoint2 * weights[2];
									skin += animPoseJoint3 * animInvBindPoseJoint3 * weights[3];

									return skin;
								};

							Triangle t1(
								wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[0], wrapedGraphics) * glm::vec4(triangleVerts[0].GetPosition(), 1.0f),
								wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[1], wrapedGraphics) * glm::vec4(triangleVerts[1].GetPosition(), 1.0f),
								wrapedGraphics->GetTransform() * skinVertexForTriangleCollider(triangleVerts[2], wrapedGraphics) * glm::vec4(triangleVerts[2].GetPosition(), 1.0f));

							if (t1.Raycast(other) != -1.0f)
							{
								intersect = true;

							}

						});
				}
			}
		});

	//if (intersect)
	//{
	//	meshColliderVisualization->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//}
	//else
	//{
	//	meshColliderVisualization->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	//}

	return intersect;
}

bool AnimatedCollider::Intersect(const SphereWithVisualization& other) const
{
	return sphere->SphereIntersect(other);
}
