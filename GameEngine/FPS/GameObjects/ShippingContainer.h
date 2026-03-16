#ifndef SHIPPINGCONTAINER_H
#define SHIPPINGCONTAINER_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class ShippingContainer : public GameObject
{
public:

	ShippingContainer();

	~ShippingContainer();

private:

	ShippingContainer(const ShippingContainer&) = delete;

	ShippingContainer& operator=(const ShippingContainer&) = delete;

	ShippingContainer(ShippingContainer&&) = delete;

	ShippingContainer& operator=(ShippingContainer&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void SetPosition(const glm::vec3& pos) override;

	glm::vec3 GetPosition() const override;

	void SetRotation(const glm::mat4& rot) override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	bool Hovered() const override;

	void Deserialize() override;

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* body;

};

#endif // SHIPPINGCONTAINER_H