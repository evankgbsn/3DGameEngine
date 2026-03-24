#ifndef WAREHOUSEROOFPANNEL_H
#define WAREHOUSEROOFPANNEL_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class WarehouseRoofPannel : public GameObject
{
public:

	WarehouseRoofPannel();

	~WarehouseRoofPannel();

private:

	WarehouseRoofPannel(const WarehouseRoofPannel&) = delete;

	WarehouseRoofPannel& operator=(const WarehouseRoofPannel&) = delete;

	WarehouseRoofPannel(WarehouseRoofPannel&&) = delete;

	WarehouseRoofPannel& operator=(WarehouseRoofPannel&&) = delete;

	// Inherited via GameObject
	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	void SetPosition(const glm::vec3&) override;

	void SetRotation(const glm::mat4&) override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	bool Hovered() const override;

	void Deserialize() override;

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* body;
};

#endif // WAREHOUSEROOFPANNEL_H