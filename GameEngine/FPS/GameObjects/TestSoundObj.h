#ifndef TESTSOUNDOBJ_H
#define TESTSOUNDOBJ_H

#include "GameEngine/GameObject/GameObject.h"

class RigidBodyComponent;
class GraphicsObjectColored;
class AudioSource;

class Source;

class TestSoundObj : public GameObject
{
public:

	TestSoundObj();

	~TestSoundObj();

private:

	TestSoundObj(const TestSoundObj&) = delete;

	TestSoundObj& operator=(const TestSoundObj&) = delete;

	TestSoundObj(TestSoundObj&&) = delete;

	TestSoundObj& operator=(TestSoundObj&&) = delete;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	bool Hovered() const override;

	void Start() override;

	glm::vec3 GetPosition() const override;

	void SetPosition(const glm::vec3& newPos) override;

	glm::mat4 GetRotation() const override;

	void SetRotation(const glm::mat4& newRot) override;

	glm::mat4 GetTransform() const override;

	RigidBodyComponent* body;

	GraphicsObjectColored* graphics;

	AudioSource* audioSource;

};

#endif // TESTSOUNDOBJ_H