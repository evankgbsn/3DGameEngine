#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include "Component.h"

class RigidBody;
class GameObject;
class Model;
class OrientedBoundingBoxWithVisualization;
class GOColored;

namespace physx
{
	class PxConvexMesh;
	class PxTriangleMesh;
}

class RigidBodyComponent : public Component
{
public:

	enum class Type
	{
		STATIC,
		DYNAMIC
	};

	RigidBodyComponent(Type type, GameObject* owner, const Model* const model);

	~RigidBodyComponent();

	void SyncPhysics();

private:

	RigidBodyComponent(const RigidBodyComponent&) = delete;

	RigidBodyComponent& operator=(const RigidBodyComponent&) = delete;

	RigidBodyComponent(RigidBodyComponent&&) = delete;

	RigidBodyComponent& operator=(RigidBodyComponent&&) = delete;

	void CreateShapeFromModel();

	RigidBody* body;

	GameObject* owner;

	const Model* const model;

	physx::PxConvexMesh* convexMeshShape;

	physx::PxTriangleMesh* triangleMeshShape;

	GOColored* shapeVisuals;

	Type type;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	void Update() override;
};

#endif // RIGIDBODYCOMPONENT_H