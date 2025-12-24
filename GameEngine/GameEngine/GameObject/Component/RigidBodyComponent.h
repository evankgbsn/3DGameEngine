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

	enum ForceMode
	{
		FORCE,				//!< parameter has unit of mass * length / time^2, i.e., a force
		IMPULSE,			//!< parameter has unit of mass * length / time, i.e., force * time
		VELOCITY_CHANGE,	//!< parameter has unit of length / time, i.e., the effect is mass independent: a velocity change.
		ACCELERATION		//!< parameter has unit of length/ time^2, i.e., an acceleration. It gets treated just like a force except the mass is not divided out before integration.
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

	void AddForce(const glm::vec3& direction, const ForceMode& forceMode = ForceMode::FORCE);

	void SetMass(float newMass);

	glm::vec3 GetVelocity() const;

	void SetPosition(const glm::vec3& newPosition);

	void SetRotation(const glm::mat4& newRot);

	bool Hovered() const;

	void SetOwner(GameObject* newOwner);

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

	void CreatePhysXRigidBodyWithoutOwner(const glm::vec3& position, const glm::mat4& rotation);

	RigidBody* body;

	GameObject* owner;

	const Model* model;

	physx::PxConvexMesh* convexMeshShape;

	physx::PxTriangleMesh* triangleMeshShape;

	GOColored* shapeVisuals;

	Type type;

	std::function<void()>* onEditorEnable;

	std::function<void()>* onEditorDisable;

	std::function<void(Model* const)>* modelLoadCallbackDynamic;

	std::function<void(Model* const)>* modelLoadCallbackStatic;

	std::string modelNameDynamic;

	std::string modelNameStatic;

	
};

#endif // RIGIDBODYCOMPONENT_H