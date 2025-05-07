#include "RigidBody.h"

#include "PhysicsManager.h"


RigidBody::RigidBody(Type t, PxGeometry* geometry, const glm::vec3& initialPosition, const glm::mat4& initialRotation) :
	type(t),
    material(nullptr),
    staticBody(nullptr),
    dynamicBody(nullptr)
{
    material = PhysicsManager::GetPhysics()->createMaterial(1.0f, 1.0f, 0.0f);

    PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;
    PxMaterial* materialPtr = material;

    PxMat33 rot(
        PxVec3(initialRotation[0][0], initialRotation[0][1], initialRotation[0][2]),
        PxVec3(initialRotation[1][0], initialRotation[1][1], initialRotation[1][2]),
        PxVec3(initialRotation[2][0], initialRotation[2][1], initialRotation[2][2])
    );

    PxShape* shape = nullptr;

    switch (type)
    {
    case Type::STATIC:

        staticBody = PhysicsManager::GetPhysics()->createRigidStatic(PxTransform(PxVec3(initialPosition.x, initialPosition.y, initialPosition.z), PxQuat(rot)));
        shape = PhysicsManager::GetPhysics()->createShape(*geometry, &materialPtr, 1, true, shapeFlags);
        staticBody->attachShape(*shape);
        shape->release(); // this way shape gets automatically released with actor

        PhysicsManager::GetScene()->addActor(*staticBody);

        break;
    case Type::DYNAMIC:

        dynamicBody = PhysicsManager::GetPhysics()->createRigidDynamic(PxTransform(PxVec3(initialPosition.x, initialPosition.y, initialPosition.z), PxQuat(rot)));
        shape = PhysicsManager::GetPhysics()->createShape(*geometry, &materialPtr, 1, true, shapeFlags);
        dynamicBody->attachShape(*shape);
        shape->release(); // this way shape gets automatically released with actor

        PhysicsManager::GetScene()->addActor(*dynamicBody);

        break;
    default:
        break;
    }
    

}

RigidBody::~RigidBody()
{
}

RigidBody::Type RigidBody::GetType() const
{
	return type;
}

PxMaterial* RigidBody::GetMaterial() const
{
    return material;
}

glm::vec3 RigidBody::GetPosition() const
{
    PxTransform transform;

    if (dynamicBody != nullptr)
    {
        transform = dynamicBody->getGlobalPose();
    }
    else if (staticBody != nullptr)
    {
        transform = staticBody->getGlobalPose();
    }

    return glm::vec3(transform.p.x, transform.p.y, transform.p.z);
}

glm::mat4 RigidBody::GetRotation() const
{
    PxTransform transform;

    if (dynamicBody != nullptr)
    {
        transform = dynamicBody->getGlobalPose();
    }
    else if (staticBody != nullptr)
    {
        transform = staticBody->getGlobalPose();
    }

    PxMat44 rot = PxMat44(transform.q);

    return glm::mat4(
        glm::vec4(rot.column0.x, rot.column0.y, rot.column0.z, rot.column0.w),
        glm::vec4(rot.column1.x, rot.column1.y, rot.column1.z, rot.column1.w),
        glm::vec4(rot.column2.x, rot.column2.y, rot.column2.z, rot.column2.w),
        glm::vec4(rot.column3.x, rot.column3.y, rot.column3.z, rot.column3.w)
    );
}
