#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

using namespace physx;

class RigidBody
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

	RigidBody(Type type, PxGeometry* geometry, const glm::vec3& initialPosition, const glm::mat4& initialRotation);

	~RigidBody();

	Type GetType() const;

	PxMaterial* GetMaterial() const;

	glm::vec3 GetPosition() const;

	glm::mat4 GetRotation() const;

	void LockAngularMotionOnAxisX();

	void LockAngularMotionOnAxisY();

	void LockAngularMotionOnAxisZ();

	void LockLinearMotionOnAxisX();

	void LockLinearMotionOnAxisY();

	void LockLinearMotionOnAxisZ();

	void AddForce(const glm::vec3& direction, const ForceMode& forceMode = ForceMode::FORCE);

	glm::vec3 GetVelocity() const;

	void SetPosition(const glm::vec3& newPosition);

	void SetRotation(const glm::mat4& newRot);

	bool Hovered() const;

	void SetUserData(void* data);

	void SetMass(float newMass);

private:

	RigidBody() = delete;

	RigidBody(const RigidBody&) = delete;

	RigidBody& operator=(const RigidBody&) = delete;

	RigidBody(RigidBody&&) = delete;

	RigidBody& operator=(RigidBody&&) = delete;

	Type type;

	PxMaterial* material;

	PxRigidDynamic* dynamicBody;

	PxRigidStatic* staticBody;
};

#endif // RIGIDBODY_H