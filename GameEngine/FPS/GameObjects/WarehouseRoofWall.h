#ifndef WAREHOUSEROOFWALL_H
#define WAREHOUSEROOFWALL_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class WarehouseRoofWall : public GameObject
{
public:

	WarehouseRoofWall();

	~WarehouseRoofWall();

private:

	WarehouseRoofWall(const WarehouseRoofWall&) = delete;

	WarehouseRoofWall& operator=(const WarehouseRoofWall&) = delete;

	WarehouseRoofWall(WarehouseRoofWall&&) = delete;

	WarehouseRoofWall& operator=(WarehouseRoofWall&&) = delete;

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


#endif // WAREHOUSEROOFWALL_H