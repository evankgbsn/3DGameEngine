#include "RigidBodyComponent.h"

#include "../../Physics/RigidBody.h"
#include "../../Physics/PhysicsManager.h"
#include "../GameObject.h"

RigidBodyComponent::RigidBodyComponent(Type type, GameObject* owningObject) :
	body(nullptr),
	owner(owningObject)
{

	PxPlaneGeometry planeGeo = PxPlaneGeometry();
	PxBoxGeometry boxGeo(1.f, 1.f, 1.f);

	switch (type)
	{
	case Type::STATIC:
		body = new RigidBody(RigidBody::Type::STATIC, &planeGeo, owningObject->GetPosition(), owningObject->GetRotation());
		break;
	case Type::DYNAMIC:
		body = new RigidBody(RigidBody::Type::DYNAMIC, &boxGeo, owningObject->GetPosition(), owningObject->GetRotation());
		break;
	default:
		break;
	}
}

RigidBodyComponent::~RigidBodyComponent()
{
	delete body;
}

void RigidBodyComponent::SyncPhysics()
{
	owner->SetPosition(body->GetPosition());
	owner->SetRotation(body->GetRotation());
}

const std::vector<char> RigidBodyComponent::Serialize() const
{
	return std::vector<char>();
}

void RigidBodyComponent::Deserialize(const std::vector<char>& data)
{
}

void RigidBodyComponent::Update()
{
}
