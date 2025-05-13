#ifndef LARGEPLANE_H
#define LARGEPLANE_H

#include "GameEngine/GameObject/GameObject.h"

class GraphicsObjectTexturedLit;
class RigidBodyComponent;
class GOTerrain;
class GraphicsObjectTexturedLit;
class GraphicsObjectTextured;

class LargePlane : public GameObject
{

public:

	LargePlane();

	~LargePlane();

private:

	LargePlane(const LargePlane&) = delete;

	LargePlane& operator=(const LargePlane&) = delete;

	LargePlane(LargePlane&&) = delete;

	LargePlane& operator=(LargePlane&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate();

	void Load() override;

	void Unload() override;

	void SetPosition(const glm::vec3& newPosition) override;

	void SetRotation(const glm::mat4& newRotation) override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	//GraphicsObjectTexturedLit* graphics;
	
	GraphicsObjectTextured* skybox;
	
	GraphicsObjectTexturedLit* graphics;

	RigidBodyComponent* rigidBody;

};

#endif // LARGEPLANE_H