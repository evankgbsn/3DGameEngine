#ifndef PLAYER_H
#define PLAYER_H

#include "GameEngine/GameObject/GameObject.h"


#include <functional>

class CameraComponent;
class RigidBodyComponent;
class GraphicsObjectTexturedAnimatedLit;
class GraphicsObjectColored;

class Player : public GameObject
{
public:

	Player();

	~Player();

	void SetPosition(const glm::vec3& newPosition) override;

	void SetRotation(const glm::mat4& newRotation) override;

	glm::vec3 GetPosition() const override;

	glm::mat4 GetRotation() const override;

	glm::mat4 GetTransform() const override;

	void Start() override;

	void End() override;

private:

	Player(const Player&) = delete;

	Player& operator=(const Player&) = delete;

	Player(Player&&) = delete;

	Player& operator=(Player&&) = delete;

	const std::vector<char> Serialize() const override;

	void Deserialize(const std::vector<char>& data) override;

	void Initialize() override;

	void Terminate() override;

	void GameUpdate() override;

	void EditorUpdate() override;

	void Load() override;

	void Unload() override;

	CameraComponent* cam;

	RigidBodyComponent* body;

	GraphicsObjectTexturedAnimatedLit* graphics;

	GraphicsObjectColored* sphere;

	std::function<void(int)>* move;

	std::function<void(const glm::vec2&)>* look;

	std::function<void(int)>* shoot;

};

#endif // PLAYER_H