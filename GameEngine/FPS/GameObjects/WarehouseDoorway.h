#ifndef WAREHOUSEDOORWAY_H
#define WAREHOUSEDOORWAY_H

#include "GameEngine/GameObject/GameObject.h"

class RigidBodyComponent;
class GraphicsObjectTexturedLit;

class WarehouseDoorway : public GameObject
{
public:

	WarehouseDoorway();

	~WarehouseDoorway();

private:

	WarehouseDoorway(const WarehouseDoorway&) = delete;

	WarehouseDoorway& operator=(const WarehouseDoorway&) = delete;

	WarehouseDoorway(WarehouseDoorway&&) = delete;

	WarehouseDoorway& operator=(WarehouseDoorway&&) = delete;

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

	GraphicsObjectTexturedLit* doorwayGraphics;

	RigidBodyComponent* doorwayBody;

};

#endif // WAREHOUSEDOORWAY_H