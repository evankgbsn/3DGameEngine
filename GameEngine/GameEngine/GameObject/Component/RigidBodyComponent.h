#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

#include <functional>

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

	RigidBodyComponent();

	RigidBodyComponent(Type type, GameObject* owner, const Model* model);

	~RigidBodyComponent();

	void SyncPhysics();

	void SyncPhysicsPosition();

	void SyncPhysicsRotation();

	void LockAngularMotionOnAxisX();

	void LockAngularMotionOnAxisY();

	void LockAngularMotionOnAxisZ();

	void LockLinearMotionOnAxisX();

	void LockLinearMotionOnAxisY();

	void LockLinearMotionOnAxisZ();

	void AddForce(const glm::vec3& direction);

	glm::vec3 GetVelocity() const;

	void SetPosition(const glm::vec3& newPosition);

	bool Hovered() const;
private:

	RigidBodyComponent(const RigidBodyComponent&) = delete;

	RigidBodyComponent& operator=(const RigidBodyComponent&) = delete;

	RigidBodyComponent(RigidBodyComponent&&) = delete;

	RigidBodyComponent& operator=(RigidBodyComponent&&) = delete;

	void CreateShapeFromModel();

	void Serialize() override;

	void Deserialize() override;

	void Update() override;

	void RegisterEditorCallbacks();

	void CreatePhysXRigidBody();

	RigidBody* body;

	GameObject* owner;

	const Model* model;

	physx::PxConvexMesh* convexMeshShape;

	physx::PxTriangleMesh* triangleMeshShape;

	GOColored* shapeVisuals;

	Type type;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	
};

#endif // RIGIDBODYCOMPONENT_H