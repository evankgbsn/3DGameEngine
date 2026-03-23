#ifndef WAREHOUSEROOFHORIZONTALBEAM_H
#define WAREHOUSEROOFHORIZONTALBEAM_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class WarehouseRoofHorizontalBeam : public GameObject
{
public:

	WarehouseRoofHorizontalBeam();

	~WarehouseRoofHorizontalBeam();

private:

	WarehouseRoofHorizontalBeam(const WarehouseRoofHorizontalBeam&) = delete;

	WarehouseRoofHorizontalBeam& operator=(const WarehouseRoofHorizontalBeam&) = delete;

	WarehouseRoofHorizontalBeam(WarehouseRoofHorizontalBeam&&) = delete;

	WarehouseRoofHorizontalBeam& operator=(WarehouseRoofHorizontalBeam&&) = delete;

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

#endif // WAREHOUSEROOFHORIZONTALBEAM_H