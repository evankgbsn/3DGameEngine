#ifndef ANIMATEDCOLLIDER_H
#define ANIMATEDCOLLIDER_H

#include "Collider.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>
#include <string>

class GO3DAnimated;
class GOColoredAnimated;
class GOColored;
class ColoredStaticGraphicsObject;
class OrientedBoundingBoxWithVisualization;
class OrientedBoundingBox;
class SphereWithVisualization;
class LineSegment3D;
class Ray;
class Vertex;
class Armature;
class StaticCollider;

class AnimatedCollider : public Collider
{
public:

	AnimatedCollider(GO3DAnimated* const graphicsObject);

	~AnimatedCollider();

	void Update() override;

	void ToggleVisibility() override;

	bool Intersect(const OrientedBoundingBox& other) const;

	bool Intersect(const LineSegment3D& other) const;

	bool Intersect(const AnimatedCollider& other) const;

	bool Intersect(const StaticCollider& other, glm::vec3& outHit) const;

	bool Intersect(const Ray& other) const;

	bool Intersect(const SphereWithVisualization& other) const;

	const SphereWithVisualization* const GetSphere() const;

private:

	void InitializeOBBs();

	void InitializeSphere();

	void InitializeMeshColliderVisualization();

	AnimatedCollider() = delete;

	AnimatedCollider(const AnimatedCollider&) = delete;

	AnimatedCollider& operator=(const AnimatedCollider) = delete;

	AnimatedCollider(AnimatedCollider&&) = delete;

	AnimatedCollider& operator=(AnimatedCollider&&) = delete;

	GO3DAnimated* wrapedGraphics;

	SphereWithVisualization* sphere;

	std::vector<std::pair<OrientedBoundingBoxWithVisualization*, unsigned int>> obbs;

	GOColoredAnimated* meshColliderVisualization;

	GOColored* collidedTriangle;

	std::unordered_map<std::string, std::vector<std::vector<Vertex>>> jointsTriangles;

	const Armature* arms;

	const std::vector<std::string>* jointNames;



};

#endif // ANIMATEDCOLLIDER_H