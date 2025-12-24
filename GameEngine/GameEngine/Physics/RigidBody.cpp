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
    if (staticBody != nullptr)
    {
        staticBody->release();
    }

    if (dynamicBody != nullptr)
    {
        dynamicBody->release();
    }
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

void RigidBody::LockAngularMotionOnAxisX()
{
    if (dynamicBody != nullptr)
    {
        dynamicBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
    }
}

void RigidBody::LockAngularMotionOnAxisY()
{
    if (dynamicBody != nullptr)
    {
        dynamicBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
    }
}

void RigidBody::LockAngularMotionOnAxisZ()
{
    if (dynamicBody != nullptr)
    {
        dynamicBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
    }
}

void RigidBody::LockLinearMotionOnAxisX()
{
    if (dynamicBody != nullptr)
    {
        dynamicBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
    }
}

void RigidBody::LockLinearMotionOnAxisY()
{
    if (dynamicBody != nullptr)
    {
        dynamicBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
    }
}

void RigidBody::LockLinearMotionOnAxisZ()
{
    if (dynamicBody != nullptr)
    {
        dynamicBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);
    }
}

void RigidBody::AddForce(const glm::vec3& direction, const ForceMode& forceMode)
{
    if (dynamicBody != nullptr)
    {
        switch (forceMode)
        {
        case ForceMode::FORCE:
            dynamicBody->addForce(PxVec3(direction.x, direction.y, direction.z), PxForceMode::eFORCE);
            break;
        case ForceMode::ACCELERATION:
            dynamicBody->addForce(PxVec3(direction.x, direction.y, direction.z), PxForceMode::eACCELERATION);
            break;
        case ForceMode::VELOCITY_CHANGE:
            dynamicBody->addForce(PxVec3(direction.x, direction.y, direction.z), PxForceMode::eVELOCITY_CHANGE);
            break;
        case ForceMode::IMPULSE:
            dynamicBody->addForce(PxVec3(direction.x, direction.y, direction.z), PxForceMode::eIMPULSE);
            break;
        default:
            break;
        }
    }
}

glm::vec3 RigidBody::GetVelocity() const
{
    if (dynamicBody != nullptr)
    {
        PxVec3 vel = dynamicBody->getLinearVelocity();
        return glm::vec3(vel.x, vel.y, vel.z);
    }
    
    return {};
}

void RigidBody::SetPosition(const glm::vec3& newPosition)
{
    PxTransform currentTransform;

    PxVec3 pos(newPosition.x, newPosition.y, newPosition.z);

    if (dynamicBody != nullptr)
    {
        currentTransform = dynamicBody->getGlobalPose();
        dynamicBody->setGlobalPose(PxTransform(pos, currentTransform.q));
    }
    else if (staticBody != nullptr)
    {
        currentTransform = staticBody->getGlobalPose();
        dynamicBody->setGlobalPose(PxTransform(pos, currentTransform.q));
    }
}

void RigidBody::SetRotation(const glm::mat4& newRot)
{
    PxTransform currentTransform;

    // Create a PxMat33 from the 3x3 rotation component of your 4x4 matrix
    PxMat33 rotationMatrix(
        PxVec3(newRot[0].x, newRot[0].y, newRot[0].z),   // Column 0
        PxVec3(newRot[1].x, newRot[1].y, newRot[1].z),   // Column 1
        PxVec3(newRot[2].x, newRot[2].y, newRot[2].z)   // Column 2
    );

    // Convert the rotation matrix to a PxQuat
    PxQuat newRotation(rotationMatrix);

    if (dynamicBody != nullptr)
    {
        currentTransform = dynamicBody->getGlobalPose();
        dynamicBody->setGlobalPose(PxTransform(currentTransform.p, newRotation));
    }
    else if (staticBody != nullptr)
    {
        currentTransform = staticBody->getGlobalPose();
        dynamicBody->setGlobalPose(PxTransform(currentTransform.p, newRotation));
    }
}

bool RigidBody::Hovered() const
{
    PxRaycastBuffer hitInfo;

    if (PhysicsManager::RaycastFromCursor(1000.0f, hitInfo))
    {
        if (dynamicBody != nullptr)
        {
            if (hitInfo.block.actor->userData == dynamicBody->userData)
            {
                return true;
            }
        }
        else if (staticBody != nullptr)
        {
            if (hitInfo.block.actor->userData == staticBody->userData)
            {
                return true;
            }
        }
        
    }

    return false;
}

void RigidBody::SetUserData(void* data)
{
    if (dynamicBody != nullptr)
    {
        dynamicBody->userData = data;
    }
    else if (staticBody != nullptr)
    {
        staticBody->userData = data;
    }
}

void RigidBody::SetMass(float newMass)
{
    if (dynamicBody != nullptr)
    {
        PxRigidBodyExt::setMassAndUpdateInertia(*dynamicBody, newMass);
    }
}
