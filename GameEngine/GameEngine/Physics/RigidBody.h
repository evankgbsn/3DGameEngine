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

	void AddForce(const glm::vec3& direction);

	glm::vec3 GetVelocity() const;

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