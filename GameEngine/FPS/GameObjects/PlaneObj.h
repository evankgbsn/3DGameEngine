#ifndef PLANEOBJ_H
#define PLANEOBJ_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;

class PlaneObj : public GameObject
{

public:
	
	PlaneObj();

	~PlaneObj();

private:

	PlaneObj(const PlaneObj&) = delete;

	PlaneObj& operator=(const PlaneObj&) = delete;

	PlaneObj(PlaneObj&&) = delete;

	PlaneObj& operator=(PlaneObj&&) = delete;

	// Inherited via GameObject
	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	void SetPosition(const glm::vec3&) override;

	void SetRotation(const glm::mat4&) override;

	bool Hovered() const override;

	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* body;
};

#endif // PLANEOBJ_H