#ifndef RIGIDBODYCOMPONENT_H
#define RIGIDBODYCOMPONENT_H

#include "Component.h"

class RigidBody;
class GameObject;

class RigidBodyComponent : public Component
{
public:

	enum class Type
	{
		STATIC,
		DYNAMIC
	};

	RigidBodyComponent(Type type, GameObject* owner);

	~RigidBodyComponent();

	void SyncPhysics();

private:

	RigidBodyComponent(const RigidBodyComponent&) = delete;

	RigidBodyComponent& operator=(const RigidBodyComponent&) = delete;

	RigidBodyComponent(RigidBodyComponent&&) = delete;

	RigidBodyComponent& operator=(RigidBodyComponent&&) = delete;

	RigidBody* body;

	GameObject* owner;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	void Update() override;
};

#endif // RIGIDBODYCOMPONENT_H