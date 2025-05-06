#include "RigidBody.h"

#include "PhysicsManager.h"

RigidBody::RigidBody(Type t, PxGeometry* geometry, const glm::vec3& initialPosition) :
	type(t),
    material(nullptr),
    staticBody(nullptr),
    dynamicBody(nullptr)
{
    material = PhysicsManager::GetPhysics()->createMaterial(1.0f, 1.0f, 0.0f);

    PxShapeFlags shapeFlags = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;
    PxMaterial* materialPtr = material;

    PxShape* shape = nullptr;

    switch (type)
    {
    case Type::STATIC:

        staticBody = PhysicsManager::GetPhysics()->createRigidStatic(PxTransformFromPlaneEquation(PxPlane(PxVec3(0.f, 1.f, 0.f), 0.f)));
        shape = PhysicsManager::GetPhysics()->createShape(*geometry, &materialPtr, 1, true, shapeFlags);
        staticBody->attachShape(*shape);
        shape->release(); // this way shape gets automatically released with actor

        PhysicsManager::GetScene()->addActor(*staticBody);

        break;
    case Type::DYNAMIC:

        dynamicBody = PhysicsManager::GetPhysics()->createRigidDynamic(PxTransform(PxVec3(initialPosition.x, initialPosition.y, initialPosition.z)));
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
    PxTransform transform = dynamicBody->getGlobalPose();

    return glm::vec3(transform.p.x, transform.p.y, transform.p.z);
}

glm::mat4 RigidBody::GetRotation() const
{
    PxTransform transform = dynamicBody->getGlobalPose();

    PxMat44 rot = PxMat44(transform.q);

    return glm::mat4(
        glm::vec4(rot.column0.x, rot.column0.y, rot.column0.z, rot.column0.w),
        glm::vec4(rot.column1.x, rot.column1.y, rot.column1.z, rot.column1.w),
        glm::vec4(rot.column2.x, rot.column2.y, rot.column2.z, rot.column2.w),
        glm::vec4(rot.column3.x, rot.column3.y, rot.column3.z, rot.column3.w)
    );
}
