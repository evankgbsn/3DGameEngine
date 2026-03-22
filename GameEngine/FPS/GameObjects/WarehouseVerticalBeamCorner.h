#ifndef WAREHOUSEVERTICALBEAMCORNER_H
#define WAREHOUSEVERTICALBEAMCORNER_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class WarehouseVerticalBeamCorner : public GameObject
{
public:

	WarehouseVerticalBeamCorner();

	~WarehouseVerticalBeamCorner();

private:

	WarehouseVerticalBeamCorner(const WarehouseVerticalBeamCorner&) = delete;

	WarehouseVerticalBeamCorner& operator=(const WarehouseVerticalBeamCorner&) = delete;

	WarehouseVerticalBeamCorner(WarehouseVerticalBeamCorner&&) = delete;

	WarehouseVerticalBeamCorner& operator=(WarehouseVerticalBeamCorner&&) = delete;

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

#endif // WAREHOUSEVERTICALBEAMCORNER_H